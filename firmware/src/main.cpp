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

extern "C" {
#include "asf.h"
#include <conf_usb.h>
}

#include "ui.h"
#include "timer.h"
#include "midi.h"
#include "midi_cmd.h"
#include "artl/timer.h"
#include "serial_cmd.h"
#include "version.h"
#include "cdc_print.h"
#include "rst_blink.h"
#include "usb_midi_buf.h"

namespace {

volatile bool cdc_enabled = false;
bool usb_midi_enabled = false;
serial_cmd_t serial_cmd;
version_t version;
usb_midi_buf_t usb_recv_buf;

usb_midi_buf_t usb_send_buf[2];
uint8_t usb_send_active = 0;
bool usb_send_busy = false;

artl::timer<> stat_timer;
uint32_t stat_period = 0;
bool stat_auto_reset = 0;
midi_cmd_t current_cmd[MIDI_PORTS];

template<typename T>
void usb_midi_send(const T& c, uint8_t jack);
bool midi_send(const usb_midi_event_t &ev, uint8_t jack, bool stall);
void midi_mon(uint8_t jack, bool dir_in, const uint8_t *b, uint8_t size);
void usb_recv_process();

void stat_print(bool auto_update);
void stat_reset();

void main_read_serial();

}

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
    bool last_dtr = false;

    irq_initialize_vectors();
    cpu_irq_enable();

    sleepmgr_init();
    sysclk_init();
    timer_init();

    ui::init();
    ui::powerdown();

    sleepmgr_lock_mode(SLEEPMGR_IDLE);
    midi::splitter();

    main_read_serial();

    // Start USB stack to authorize VBus monitoring
    udc_start();

    ui::startup_animation();

    unsigned long t = millis();

    // The main loop manages only the power mode
    // because the USB management is done by interrupt
    while (true) {
        sleepmgr_enter_sleep();

        if (tc_flag) {
            tc_flag = 0;

            t = millis();

            if (ui::btn_update(t)) {
                if (ui::btn_down()) {
                    rst_blink_start(t);
                } else {
                    if (rst_blink_count < 2) {
                        reset_do_soft_reset();
                    }

                    if (usb_midi_enabled) {
                        udc_stop();
                    } else {
                        udc_start();
                    }

                    rst_blink_stop();
                }
            } else {
                rst_blink_update(t);
            }

            if (!usb_midi_enabled && midi::rx_ready) {
                midi::rx_ready = 0;

                ui::rx_blink(0);
                ui::tx_blink();
            }

            if (stat_timer.update(t)) {
                stat_timer.schedule(t + stat_period);

                stat_print(true);
            }
        }

        if (last_dtr != cdc_dtr) {
            last_dtr = cdc_dtr;

            if (cdc_dtr) {
                _cdc_println(USB_DEVICE_PRODUCT_NAME);

                for (uint8_t l = 0; l < version_t::MAX_LINE; ++l) {
                    _cdc_println(version.lines[l]);
                }

                _cdc_prompt();
            } else {
                midi::stop_mon();
                stat_timer.cancel();
                stat_period = 0;
            }
        }
    }
}

void midi_process_byte(uint8_t port, uint8_t data) {
    ++midi::port_stat[port].rcv_bytes;
    midi::port_stat_update = true;

    if (data == CMD_SYS_ACTIVE_S) {
        ui::rx_active(port);
    } else {
        ui::rx_blink(port);
    }

    if (is_midi_rt(data)) {
        midi_cmd_t cmd;
        cmd.read(data);

        midi_mon(port, true, cmd, cmd.size());

        usb_midi_send(cmd, port);

        ++midi::port_stat[port].rcv_msgs;
        return;
    }

    if (is_midi_cmd(data) && data != CMD_SYS_EX_END && current_cmd[port].sys_ex()) {
        usb_midi_send(current_cmd[port], port);
        current_cmd[port].reset();
    }

    current_cmd[port].read(data);
    if (current_cmd[port].ready()) {
        usb_midi_send(current_cmd[port], port);
        current_cmd[port].reset();

        ++midi::port_stat[port].rcv_msgs;
    }
}

void midi_send_ready(uint8_t port, uint8_t size) {
    if (!usb_recv_buf.empty()) {
        usb_recv_process();
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
    cdc_enabled = true;
    return true;
}

void main_cdc_disable(uint8_t port)
{
    cdc_enabled = false;
}

void main_cdc_set_dtr(uint8_t port, bool b_enable)
{
    cdc_dtr = b_enable;
}

void process_serial_cmd(uint8_t port) {
    switch(serial_cmd.command()) {
    case serial_cmd_t::CMD_UNKNOWN:
        break;

    case serial_cmd_t::CMD_HELP:
        for (uint16_t i = 0; i < sizeof(help_); ++i) {
            char c = PROGMEM_READ_BYTE(help_ + i);

            if (c == '\n') udi_cdc_putc('\r');

            udi_cdc_putc(c);
        }
        break;

    case serial_cmd_t::CMD_VERSION:
        cdc_println(USB_DEVICE_PRODUCT_NAME);

        for (uint8_t l = 0; l < version_t::MAX_LINE; ++l) {
            cdc_println(version.lines[l]);
        }
        break;

    case serial_cmd_t::CMD_SERIAL_NUMBER:
        cdc_println("SN ", version.sn());
        break;

    case serial_cmd_t::CMD_HARDWARE: {
        char v[10];

        if (serial_cmd.get_arg(1, v)) {
            nvm_eeprom_erase_and_write_buffer(EEPROM_START, v, sizeof(v));

            version.hw(v);
        }

        cdc_println("HW ", version.hw());
        break;
    }

    case serial_cmd_t::CMD_UPTIME:
        cdc_println("uptime ", millis(), " ms");
        break;

    case serial_cmd_t::CMD_MIDI_MON:
        midi::start_mon();
        break;

    case serial_cmd_t::CMD_PORT_STAT: {
        uint32_t d;
        char r[1];

        if (serial_cmd.get_arg(1, d) && d != 0) {
            stat_period = d * 1000;

            stat_timer.schedule(millis() + stat_period);

            stat_auto_reset = serial_cmd.get_arg(2, r) && r[0] == 'R';
        } else {
            stat_period = 0;

            stat_auto_reset = serial_cmd.get_arg(1, r) && r[0] == 'R';
        }

        stat_print(false);
        break;
    }

    case serial_cmd_t::CMD_PORT_STAT_RESET: {
        stat_reset();
        break;
    }

    default:
        cdc_println("ERR");
    }

    serial_cmd.reset();
}

void main_cdc_rx_notify(uint8_t port) {
    static uint8_t buf[16];
    static iram_size_t size, i;

    size = udi_cdc_multi_read_no_polling(port, buf, sizeof(buf));

    for (i = 0; i < size; ++i) {
        if (midi::port_mon || stat_period != 0) {
            if (buf[i] == '\n' || buf[i] == '\r') {
                midi::stop_mon();
                stat_timer.cancel();
                stat_period = 0;

                _cdc_prompt();
            }

            continue;
        }

        udi_cdc_putc(buf[i]);

        if (buf[i] == '\r') udi_cdc_putc('\n');

        serial_cmd.read(buf[i]);

        if (serial_cmd) {
            process_serial_cmd(port);

            if (!midi::port_mon && stat_period == 0) {
                _cdc_prompt();
            }
        }
    }
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

namespace {

void stat_print(bool auto_update) {
    enum {
        MAX_FIELD = midi::port_stat_t::MAX_FIELD,
    };

    const char *t[MAX_FIELD] = {
        "Rbyte", "Rmsg",
        "Sbyte", "Smsg", "Sovf",
        "STms", "STbyte", "STmsg" };

    uint8_t w[MAX_FIELD];

    if (auto_update) {
        if (!midi::port_stat_update) {
            return;
        }

        cdc_println("uptime ", millis(), " ms");
    }

    midi::port_stat_update = false;

    for (uint8_t j = 0; j < MAX_FIELD; ++j) {
        w[j] = 1 + strlen(t[j]);
    }

    for (uint8_t i = 0; i <= MIDI_PORTS; ++i) {
        const midi::port_stat_t &s = midi::port_stat[i];

        for (uint8_t j = 0; j < MAX_FIELD; ++j) {
            uint8_t m = 1 + cdc_get_width(s[j]);
            if (m > w[j]) { w[j] = m; }
        }
    }

    _cdc_print('#');
    for (uint8_t j = 0; j < MAX_FIELD; ++j) {
        _cdc_print_w(t[j], w[j]);
    }
    _cdc_println();

    for (uint8_t i = 0; i <= MIDI_PORTS; ++i) {
        const midi::port_stat_t &s = midi::port_stat[i];

        if (i < MIDI_PORTS) {
            _cdc_print(i);
        } else {
            _cdc_print('u');
        }

        for (uint8_t j = 0; j < MAX_FIELD; ++j) {
            _cdc_print_w(s[j], w[j]);
        }
        _cdc_println();
    }

    if (stat_auto_reset) {
        stat_reset();
    }
}

void stat_reset() {
    for (uint8_t i = 0; i < MIDI_PORTS + 1; ++i) {
        midi::port_stat[i].reset();
    }
}

bool midi_send(const usb_midi_event_t &ev, uint8_t jack, bool stall) {
    uint8_t s = ev.size();

    if (s == 0) return true;

    midi::port_stat_t &stat = midi::port_stat[jack];

    uint8_t res = midi::send(jack, ev, s);

    if (res != s) {
        if (stat.stall_start == 0) {
            stat.stall_start = millis();
        }

        return false;
    }

    stat.snd_bytes += res;
    ++stat.snd_msgs;

    midi::port_stat_update = true;

    if (stall != 0) {
        stat.stall_bytes += res;
        ++stat.stall_msgs;
    }

    if (stat.stall_start != 0) {
        stat.stall_ms += millis() - stat.stall_start;

        stat.stall_start = 0;
    }

    midi_mon(jack, false, ev, ev.size());

    if (ev.byte1 == CMD_SYS_ACTIVE_S) {
        ui::rx_usb_active();
        ui::tx_active(jack);
    } else {
        ui::rx_usb_blink();
        ui::tx_blink(jack);
    }

    return true;
}

void usb_midi_received(udd_ep_status_t status, iram_size_t n, udd_ep_id_t ep)
{
    usb_recv_buf.received(n);

    midi::port_stat[MIDI_PORTS].rcv_bytes += n;
    midi::port_stat[MIDI_PORTS].rcv_msgs += n / sizeof(usb_midi_event_t);

    usb_recv_process();
}

bool udi_midi_enable(void) {
    if (!usb_midi_enabled) {
        usb_midi_enabled = true;
        sleepmgr_unlock_mode(SLEEPMGR_IDLE);
    }

    ui::usb_midi_enable();
    midi::init();

    udd_ep_run(UDI_MIDI_EP_OUT,
        true,
        usb_recv_buf,
        usb_recv_buf.capacity(),
        usb_midi_received);

    return true;
}

void usb_recv_process() {
    while (!usb_recv_buf.empty()) {
        const usb_midi_event_t& ev = usb_recv_buf.first();
        if (midi_send(ev, ev.jack(), usb_recv_buf.stall)) {
            usb_recv_buf.pop();
        } else {
            usb_recv_buf.set_stall();
            return;
        }
    }

    udd_ep_run(UDI_MIDI_EP_OUT,
        true,
        usb_recv_buf,
        usb_recv_buf.capacity(),
        usb_midi_received);
}

void udi_midi_disable(void) {
    if (usb_midi_enabled) {
        usb_midi_enabled = false;
        sleepmgr_lock_mode(SLEEPMGR_IDLE);
    }

    ui::usb_midi_disable();
    midi::splitter();
}

void usb_midi_flush();

void usb_midi_sent(udd_ep_status_t status, iram_size_t n, udd_ep_id_t ep)
{
    if (status == UDD_EP_TRANSFER_OK) {
        midi::port_stat[MIDI_PORTS].snd_msgs += n / sizeof(usb_midi_event_t);
        midi::port_stat[MIDI_PORTS].snd_bytes += n;
    } else {
        ++midi::port_stat[MIDI_PORTS].stall_msgs;
    }

    usb_send_busy = false;

    if (!usb_send_buf[usb_send_active].empty()) {
        usb_midi_flush();
    }
}

void usb_midi_flush() {
    uint8_t *buf = usb_send_buf[usb_send_active];
    uint8_t size = usb_send_buf[usb_send_active].size();

    usb_send_active = (usb_send_active + 1) % 2;
    usb_send_buf[usb_send_active].reset();

    usb_send_busy = true;

    if (udd_ep_run(UDI_MIDI_EP_IN, true, buf, size, usb_midi_sent)) {

    } else {
        ++midi::port_stat[MIDI_PORTS].stall_msgs;
        usb_send_busy = false;
    }
}


inline void copy(usb_midi_event_t& ev, const midi_cmd_t& c, uint8_t jack) {
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
}

void midi_mon(uint8_t jack, bool dir_in, const uint8_t *b, uint8_t size) {
    if (cdc_dtr &&
        ((dir_in && midi::port_in_mon[jack]) ||
         (!dir_in && midi::port_out_mon[jack]))) {
        iram_size_t csize = udi_cdc_get_free_tx_buffer();

        if (csize >= (iram_size_t) (10 + 3 + 4 * size + 2) * 2) {
            _cdc_print(millis(), ' ', dir_in ? 'i' : 'o', jack, ' ', b[0]);

            for (uint8_t i = 1; i < size; ++i) {
                _cdc_print(' ', b[i]);
            }

            _cdc_println();
        } else {
            if (csize >= 1) {
                _cdc_print('.');
            }
        }
    }
}

inline void copy(usb_midi_event_t& ev, const usb_midi_event_t& c, uint8_t jack) {
    ev.header = (c.header & 0x0F) | (jack << 4);
    ev.byte1 = c.byte1;
    ev.byte2 = c.byte2;
    ev.byte3 = c.byte3;
}

template<typename T>
void usb_midi_send(const T& c, uint8_t jack)
{
    if (usb_send_buf[usb_send_active].full()) {
        ++midi::port_stat[MIDI_PORTS].snd_ovf;
        return;
    }

    usb_midi_event_t &ev = usb_send_buf[usb_send_active].push();

    copy(ev, c, jack);

    if (midi_cmd_t::command(c[0]) == CMD_SYS_ACTIVE_S) {
        ui::tx_usb_active();
    } else {
        ui::tx_usb_blink();
    }

    if (!usb_send_busy) {
        usb_midi_flush();
    }
}

bool udi_midi_setup(void) {
    return false;
}

uint8_t udi_midi_getsetting(void) {
    return 0;
}

}

//! Global structure which contains standard UDI API for UDC
UDC_DESC_STORAGE udi_api_t udi_api_midi = {
    .enable = udi_midi_enable,
    .disable = udi_midi_disable,
    .setup = udi_midi_setup,
    .getsetting = udi_midi_getsetting,
    .sof_notify = NULL,
};

namespace {

//! Storage for serial number
struct nvm_device_serial serial;
uint8_t serial_str[16];

uint8_t hex_str(uint8_t b) {
    if (b < 10) return '0' + b;
    return 'A' + b;
}

void main_read_serial() {
    nvm_read_device_serial(&serial);

    memcpy(serial_str, serial.byte, 6);

    uint8_t *c = serial_str + 6;

    for (uint8_t i = 6; i < sizeof(serial.byte); ++i) {
        *c++ = hex_str(serial.byte[i] >> 4);
        *c++ = hex_str(serial.byte[i] & 0x0F);
    }

    version.sn(serial_str);

    char v[10];
    nvm_eeprom_read_buffer(EEPROM_START, v, sizeof(v));
    version.hw(v);
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
