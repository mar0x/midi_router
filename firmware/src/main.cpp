/**
 * \file
 *
 * \brief Main functions for USB composite example
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <conf_usb.h>

#include "ui.h"
#include "timer.h"
#include "crit_sec.h"
#include "midi.h"
#include "midi_cmd.h"
#include "artl/timer.h"

static volatile bool main_b_cdc_enabled = false;
static bool main_b_cdc_dtr = false;
static bool main_b_usb_midi_enabled = false;

static void cdc_print_eol(void);
static void cdc_print(uint16_t n);
//static void cdc_print_l(unsigned long n);

static void usb_midi_dispatch(const midi_cmd_t& c, uint8_t jack);

static void main_read_serial();

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
    irq_initialize_vectors();
    cpu_irq_enable();

    // Initialize the sleep manager
    sleepmgr_init();

    sysclk_init();
    //board_init();

    timer_init();

    ui::init();
    ui::powerdown();

    midi::splitter();

    main_read_serial();

    // Start USB stack to authorize VBus monitoring
    udc_start();

    ui::startup_animation();

    unsigned long t = millis();

    uint8_t rst_count = 0;
    artl::timer<> rst_blink;
    midi_cmd_t current_cmd[7];

    // The main loop manages only the power mode
    // because the USB management is done by interrupt
    while (true) {
        sleepmgr_enter_sleep();

        if (tc_flag) {
            tc_flag = 0;

            t = millis();

            if (ui::btn_update(t)) {
                if (ui::btn_down()) {
                    ui::rst_blink();
                    rst_blink.schedule(t + 600);
                } else {
                    if (rst_count < 2) {
                        CCP = CCP_IOREG_gc; // see AU manual, sect 3.14.1 (protected I/O)
                        RST.CTRL |= RST_SWRST_bm;
                    }

                    rst_count = 0;
                    rst_blink.cancel();
                }
            } else {
                if (ui::btn_down() && rst_blink.update(t)) {
                    rst_blink.schedule(t + 600);

                    ui::rst_blink();

                    rst_count++;

                    if (rst_count >= 5) {
                        RST.STATUS |= RST_SRF_bm;

                        CCP = CCP_IOREG_gc; // see AU manual, sect 3.14.1 (protected I/O)
                        WDT.CTRL = WDT_ENABLE_bm | WDT_CEN_bm | WDT_PER_8CLK_gc;

                        for (;;) ;
                    }
                }
            }
        }

        if (main_b_usb_midi_enabled) {
            uint8_t port, data;

            while (midi::recv(port, data)) {
                ui::rx_blink(port);

                if (is_midi_rt(data)) {
                    midi_cmd_t cmd;
                    cmd.read(data);
                    usb_midi_dispatch(cmd, port);
                    continue;
                }

                current_cmd[port].read(data);
                if (current_cmd[port].ready()) {
                    usb_midi_dispatch(current_cmd[port], port);
                    current_cmd[port].reset();
                }
            }
        } else {
            if (midi::rx_ready) {
                midi::rx_ready = 0;

                ui::rx_blink(0);
                ui::tx_blink();
            }
        }
    }
}

void main_suspend_action(void)
{
    ui::powerdown();
}

void main_resume_action(void)
{
    ui::wakeup();
}

void main_sof_action(void)
{
}

bool main_cdc_enable(uint8_t port)
{
    main_b_cdc_enabled = true;
    return true;
}

void main_cdc_disable(uint8_t port)
{
    main_b_cdc_enabled = false;
}

void main_cdc_set_dtr(uint8_t port, bool b_enable)
{
    if (b_enable) {
        main_b_cdc_dtr = true;
    } else {
        main_b_cdc_dtr = false;
    }
}

void main_cdc_rx_notify(uint8_t port) {
}

void main_cdc_config(uint8_t port, usb_cdc_line_coding_t * cfg) {
}

namespace {

bool udi_audio_ctrl_enable(void) {
    return true;
}

void udi_audio_ctrl_disable(void) {
}

bool udi_audio_ctrl_setup(void) {
    return false;
}

uint8_t udi_audio_ctrl_getsetting(void) {
    return 0;
}

}

//! Global structure which contains standard UDI API for UDC
UDC_DESC_STORAGE udi_api_t udi_api_audio_ctrl = {
    .enable = udi_audio_ctrl_enable,
    .disable = udi_audio_ctrl_disable,
    .setup = udi_audio_ctrl_setup,
    .getsetting = udi_audio_ctrl_getsetting,
    .sof_notify = NULL,
};

uint8_t midi_buf[64];

static void cdc_print_eol(void) {
    if (!main_b_cdc_dtr) return;

    udi_cdc_putc('\r');
    udi_cdc_putc('\n');
}

static void cdc_print(uint16_t n) {
    char buf[5];
    uint8_t s = sizeof(buf) - 1;

    if (!main_b_cdc_dtr) return;

    do {
        uint16_t c = n % 10;
        n = n / 10;
        buf[s--] = '0' + c;
    } while (n > 0);

    for(s++; s < sizeof(buf); s++) {
        udi_cdc_putc(buf[s]);
    }
}

struct usb_midi_event_t {
    uint8_t header;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;

    void reset() { header = 0; }
    bool empty() const { return header == 0; }

    operator bool() const { return header != 0; }
};

bool midi_dispatch(const usb_midi_event_t &ev) {
    if (!ev.header) return false;

    uint8_t s = 0;
    uint8_t cin = ev.header & 0x0F;
    switch (cin) {
    case 0:
    case 1:
        return false;
    case 5:
        s = 1;
        break;
    case 2:
    case 6:
    case 0xC:
    case 0xD:
        s = 2;
        break;
    case 3:
    case 4:
    case 7:
    case 8:
    case 9:
    case 0xA:
    case 0xB:
    case 0xE:
        s = 3;
        break;
    }

    uint8_t jack = (ev.header & 0xF0) >> 4;

    midi::send(jack, &ev.byte1, s);
    ui::tx_blink(jack);

    return true;
}

static void usb_midi_received(udd_ep_status_t status, iram_size_t n, udd_ep_id_t ep)
{
    if (n > 0) { ui::rx_usb_blink(); }

    if (main_b_cdc_dtr) {
        cdc_print(millis());
        udi_cdc_putc(' ');
        cdc_print(n);
        udi_cdc_putc(':');
        for (uint16_t i = 0; i < n; i++) {
            cdc_print(midi_buf[i]);
            udi_cdc_putc(',');
        }
        cdc_print_eol();
    }

    usb_midi_event_t *ev = (usb_midi_event_t *) midi_buf;
    n /= sizeof(usb_midi_event_t);

    for (uint8_t i = 0; i < n; ++i) {
        midi_dispatch(ev[i]);
    }

    udd_ep_run(UDI_MIDI_EP_OUT,
        true,
        midi_buf,
        sizeof(midi_buf),
        usb_midi_received);
}

static bool udi_midi_enable(void) {
    main_b_usb_midi_enabled = true;

    ui::usb_midi_enable();
    midi::init();

    udd_ep_run(UDI_MIDI_EP_OUT,
        true,
        midi_buf,
        sizeof(midi_buf),
        usb_midi_received);

    return true;
}

static void udi_midi_disable(void) {
    main_b_usb_midi_enabled = false;

    ui::usb_midi_disable();
    midi::splitter();
}

/*
static usb_midi_event_t usb_midi_ev_ring[16];
static uint8_t usb_midi_ev_idx = 0;
*/

static void usb_midi_sent(udd_ep_status_t status, iram_size_t n, udd_ep_id_t ep)
{
}

static void usb_midi_dispatch(const midi_cmd_t& c, uint8_t jack)
{
    static usb_midi_event_t ev;
    //usb_midi_event_t *ev = &usb_midi_ev_ring[usb_midi_ev_idx];

    //usb_midi_ev_idx = (usb_midi_ev_idx + 1) % 16;

    switch (c.command()) {
    case 0:
    case CMD_SYS_EX:
        if (c.size() == 3) {
            ev.header = (c[2] == CMD_SYS_EX_END) ? 0x07 : 0x04;
        } else {
            ev.header = 0x06;
        }
        break;

    case CMD_NOTE_OFF:
    case CMD_NOTE_ON:
    case CMD_KEY_PRESSURE:
    case CMD_CTRL_CHANGE:
    case CMD_PROG_CHANGE:
    case CMD_CHAN_PRESSURE:
    case CMD_PITCH_CHANGE:
        ev.header = c.command() >> 4;
        break;

    case CMD_SYS_MTC:
    case CMD_SYS_SONG_SEL:
        ev.header = 0x2;
        break;

    case CMD_SYS_SONG_PP:
        ev.header = 0x3;
        break;

    case CMD_SYS_TUNE_REQ:
    case CMD_SYS_EX_END:
        ev.header = 0x5;
        break;

    case CMD_SYS_CLOCK:
    case CMD_SYS_TICK:
    case CMD_SYS_START:
    case CMD_SYS_CONT:
    case CMD_SYS_STOP:
    case CMD_SYS_UNDEF:
    case CMD_SYS_ACTIVE_S:
    case CMD_SYS_RESET:
        ev.header = 0xF;
        break;

    default:
        ev.header = 0;
        break;
    }

    ev.header |= (jack << 4);
    ev.byte1 = c[0];
    ev.byte2 = c.size() > 1 ? c[1] : 0;
    ev.byte3 = c.size() > 2 ? c[2] : 0;

    if (main_b_cdc_dtr) {
        cdc_print(millis());
        udi_cdc_putc(':');
        udi_cdc_putc('u');
        cdc_print(ev.header);
        udi_cdc_putc(',');
        cdc_print(ev.byte1);
        udi_cdc_putc(',');
        cdc_print(ev.byte2);
        udi_cdc_putc(',');
        cdc_print(ev.byte3);
        cdc_print_eol();
    }

    ui::tx_usb_blink();

    udd_ep_run(UDI_MIDI_EP_IN,
        true,
        (uint8_t *) &ev,
        sizeof(ev),
        usb_midi_sent);
}

static bool udi_midi_setup(void) {
    return false;
}

static uint8_t udi_midi_getsetting(void) {
    return 0;
}

//! Global structure which contains standard UDI API for UDC
UDC_DESC_STORAGE udi_api_t udi_api_midi = {
    .enable = udi_midi_enable,
    .disable = udi_midi_disable,
    .setup = udi_midi_setup,
    .getsetting = udi_midi_getsetting,
    .sof_notify = NULL,
};

//! Storage for serial number
static struct nvm_device_serial serial;
static uint8_t serial_str[16];

static uint8_t hex_str(uint8_t b) {
    if (b < 10) return '0' + b;
    return 'A' + b;
}

static void main_read_serial() {
    nvm_read_device_serial(&serial);

    memcpy(serial_str, serial.byte, 6);

    uint8_t *c = serial_str + 6;

    for (uint8_t i = 6; i < sizeof(serial.byte); ++i) {
        *c++ = hex_str(serial.byte[i] >> 4);
        *c++ = hex_str(serial.byte[i] & 0x0F);
    }
}

const uint8_t *main_serial_name() {
    return serial_str;
}

/**
 * \mainpage ASF USB Composite Device Example CDC and MSC
 *
 * \section intro Introduction
 * This example shows how to implement a USB Composite Device with CDC and
 * Mass Storage interfaces on Atmel MCU with USB module.
 *
 * \section desc Description of the Communication Device Class (CDC)
 * The Communication Device Class (CDC) is a general-purpose way to enable all
 * types of communications on the Universal Serial Bus (USB).
 * This class makes it possible to connect communication devices such as
 * digital telephones or analog modems, as well as networking devices
 * like ADSL or Cable modems.
 * While a CDC device enables the implementation of quite complex devices,
 * it can also be used as a very simple method for communication on the USB.
 * For example, a CDC device can appear as a virtual COM port, which greatly
 * simplifies application development on the host side.
 *
 * \section startup Startup
 * The example uses all memories available on the board and connects these to
 * USB Device Mass Storage stack.
 * Also, the example is a bridge between a USART from the main MCU
 * and the USB CDC interface.
 * After loading firmware, connect the board (EVKxx,Xplain,...) to the USB Host.
 * When connected to a USB host system this application allows to display
 * all available memories as a removable disks and provides a mouse, keyboard and CDC in
 * the Unix/Mac/Windows operating systems.
 *
 * In this example, we will use a PC as a USB host:
 * it connects to the USB and to the USART board connector.
 * - Connect the USART peripheral to the USART interface of the board.
 * - Connect the application to a USB host (e.g. a PC)
 *   with a mini-B (embedded side) to A (PC host side) cable.
 * The application will behave as a virtual COM (see Windows Device Manager).
 * - Open a HyperTerminal on both COM ports (RS232 and Virtual COM)
 * - Select the same configuration for both COM ports up to 115200 baud.
 * - Type a character in one HyperTerminal and it will echo in the other.
 *
 * \note
 * This example uses the native MSC driver on Unix/Mac/Windows OS, except for Win98.
 * About CDC, on the first connection of the board on the PC,
 * the operating system will detect a new peripheral:
 * - This will open a new hardware installation window.
 * - Choose "No, not this time" to connect to Windows Update for this installation
 * - click "Next"
 * - When requested by Windows for a driver INF file, select the
 *   atmel_devices_cdc.inf file in the directory indicated in the Atmel Studio
 *   "Solution Explorer" window.
 * - click "Next"
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and CDC & MSC modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/msc/
 *   <br>services/usb/class/cdc/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - udi_composite_desc.c,udi_composite_conf.h,
 *      <br>USB Composite Device definition
 *    - uart_uc3.c,
 *      <br>implementation of RS232 bridge for AVR32 parts
 *    - uart_xmega.c,
 *      <br>implementation of RS232 bridge for XMEGA parts
 *    - uart_sam.c,
 *      <br>implementation of RS232 bridge for SAM parts
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (buttons, leds)
 *
 * <SUP>1</SUP> The memory data transfers are done outside USB interrupt routine.
 * This is done in the MSC process ("udi_msc_process_trans()") called by main loop.
 */
