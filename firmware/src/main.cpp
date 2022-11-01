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
#include "usb.h"

bool cdc_dtr = false;

namespace {

volatile bool cdc_enabled = false;
bool usb_midi_enabled = false;
serial_cmd_t serial_cmd;
version_t version;

artl::timer<> stat_timer;
artl::timer<> active_test_timer;
uint32_t stat_period = 0;
bool stat_auto_reset = 0;
uint8_t stat_port_start = 0;

void usb_midi_enable(bool enable);

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

    ui::tx_blink_state[1].write(true);

    ui::powerdown();

    ui::tx_blink_state[2].write(true);

    sleepmgr_lock_mode(SLEEPMGR_IDLE);
    midi::setup();

    ui::tx_blink_state[3].write(true);

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
                    switch(rst_blink_count) {
                    case 0:
                    case 1:
                        reset_do_soft_reset();
                        break;

                    case 2:
                        ui::led_test_disable();
                        active_test_timer.cancel();

                        usb_midi_enable(!usb_midi_enabled);
                        break;

                    case 3:
                        ui::led_test_enable();
                        active_test_timer.cancel();
                        break;

                    case 4:
                        ui::led_test_disable();
                        active_test_timer.schedule(t + 300);
                        break;
                    }

                    rst_blink_stop();
                }
            } else {
                rst_blink_update(t);
            }

            if (stat_port_start != 0) {
                if (cdc_dtr) {
                    stat_print(false);
                }
                if (stat_period == 0 && stat_port_start == 0) {
                    cdc_prompt();
                }
            }

            if (stat_timer.update(t)) {
                stat_timer.schedule(t + stat_period);

                stat_print(true);
            }

            if (active_test_timer.update(t)) {
                active_test_timer.schedule(t + 300);

                uint8_t b[] = { CMD_SYS_ACTIVE_S, CMD_SYS_TICK };

                if (usb_midi_enabled) {
                    for(uint8_t i = 0; i < MIDI_OUT_PORTS; ++i) {
                        bool res = midi::mixer[i](&b[i % 2], 1, CMDID_SYS_ACTIVE_S);
                        if (res) {
                            midi::port_stat_t &stat = midi::port_stat[i];

                            ++stat.snd_bytes;
                            ++stat.snd_msgs;

                            ui::tx_data((i % 2) == 0, i);
                        }
                    }
                }
            }

            midi::update_timer(t);
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

void midi_send_ready(uint8_t port, uint8_t size) {
    usb::process_rx();
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
        uint8_t rarg = 1;

        if (serial_cmd.get_arg(1, d) && d != 0) {
            stat_period = d * 1000;
            rarg = 2;

            stat_timer.schedule(millis() + stat_period);
        } else {
            stat_period = 0;
            rarg = 1;
        }

        stat_auto_reset = serial_cmd.get_arg(rarg, r) && r[0] == 'R';

        stat_print(false);
        break;
    }

    case serial_cmd_t::CMD_PORT_STAT_RESET:
        stat_reset();
        break;

    case serial_cmd_t::CMD_MODE_SPLITTER:
        ui::led_test_disable();
        active_test_timer.cancel();
        usb_midi_enable(false);
        break;

    case serial_cmd_t::CMD_MODE_ROUTER:
        ui::led_test_disable();
        active_test_timer.cancel();
        usb_midi_enable(true);
        break;

    case serial_cmd_t::CMD_MODE_LED_TEST:
        ui::led_test_enable();
        active_test_timer.cancel();
        break;

    case serial_cmd_t::CMD_MODE_ACTIVE_TEST:
        ui::led_test_disable();
        active_test_timer.schedule(millis() + 300);
        break;

    case serial_cmd_t::CMD_CONF_FILTER: {
        char t[2];
        uint8_t n;

        if (serial_cmd.get_arg(1, t)) {
            if (serial_cmd.get_arg(2, n)) {
                _cdc_print("CF ", t[0], ' ', n, ' ');

                midi::filter_t *f;
                uint8_t max_n;

                switch (t[0]) {
                case 'I': f = midi::in_port_filter; max_n = MIDI_IN_PORTS; break;
                case 'O': f = midi::out_port_filter; max_n = MIDI_OUT_PORTS; break;
                case 'R': f = midi::route_filter; max_n = MIDI_ROUTES; break;
                default: f = NULL; max_n = 0;
                }

                if (n < max_n) {
                    _cdc_print_hex(f[n].command_filter);
                    _cdc_print(' ');
                    _cdc_print_hex(f[n].channel_filter);
                }
                _cdc_print_eol();
            }
        }
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
        if (midi::mon_enabled || stat_period != 0) {
            if (buf[i] == '\n' || buf[i] == '\r') {
                if (midi::mon_enabled) {
                    _cdc_print_eol();
                }

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

            if (!midi::mon_enabled && stat_period == 0 && stat_port_start == 0) {
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

enum {
    MAX_FIELD = midi::port_stat_t::MAX_FIELD,
};

uint8_t stat_field_width[MAX_FIELD];
uint16_t stat_line_width = 0;

void stat_update_field_width() {
    stat_line_width = 1 + 2; // # + eol

    for (uint8_t j = 0; j < MAX_FIELD; ++j) {
        stat_field_width[j] = 1 + midi::port_stat_t::title_len[j];

        for (uint8_t i = 0; i <= MIDI_PORTS; ++i) {
            const midi::port_stat_t &s = midi::port_stat[i];

            uint8_t m = 1 + cdc_get_width(s[j]);
            if (m > stat_field_width[j]) { stat_field_width[j] = m; }
        }

        stat_line_width += stat_field_width[j];
    }
}

void stat_print(bool auto_update) {
    if (stat_port_start == 0) {
        if (auto_update) {
            if (!midi::port_stat_update) {
                return;
            }

            cdc_println("uptime ", millis(), " ms");
        }
    }

    if (midi::port_stat_update || stat_line_width == 0) {
        stat_update_field_width();
    }

    iram_size_t csize = udi_cdc_get_free_tx_buffer();

    if (csize < 2 * (stat_line_width + 3)) return;

    if (stat_port_start == 0) {
        midi::port_stat_update = false;

        _cdc_print('#');
        for (uint8_t j = 0; j < MAX_FIELD; ++j) {
            _cdc_print_w(midi::port_stat_t::title[j], stat_field_width[j]);
        }
        _cdc_println();

        csize -= stat_line_width + 3;
    }

    for (uint8_t i = stat_port_start; i <= MIDI_PORTS; ++i) {
        if (csize < stat_line_width + 3) {
            stat_port_start = i;
            return;
        }

        const midi::port_stat_t &s = midi::port_stat[i];

        if (i < MIDI_PORTS) {
            _cdc_print(i);
        } else {
            _cdc_print('u');
        }

        for (uint8_t j = 0; j < MAX_FIELD; ++j) {
            _cdc_print_w(s[j], stat_field_width[j]);
        }
        _cdc_println();

        csize -= stat_line_width + 3;
    }

    stat_port_start = 0;

    midi::dump_state();

    if (stat_auto_reset) {
        stat_reset();
    }
}

void stat_reset() {
    for (uint8_t i = 0; i < MIDI_PORTS + 1; ++i) {
        midi::port_stat[i].reset();
    }
}

void usb_midi_enable(bool enable) {
    if (usb_midi_enabled == enable) {
        return;
    }

    usb_midi_enabled = enable;

    if (enable) {
        sleepmgr_unlock_mode(SLEEPMGR_IDLE);

        ui::usb_midi_enable();
    } else {
        sleepmgr_lock_mode(SLEEPMGR_IDLE);

        ui::usb_midi_disable();
    }
}

bool udi_midi_enable(void) {
    usb_midi_enable(true);

    usb::enable();

    return true;
}

void udi_midi_disable(void) {
    usb_midi_enable(false);

    cdc_dtr = false;
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
