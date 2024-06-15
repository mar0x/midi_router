/**
 * \file
 *
 * \brief User Interface
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

#include <ui.h>

#include "led.h"
#include <artl/button.h>
#include <artl/digital_in.h>
#include <timer.h>
#include <artl/timer.h>

namespace {

using btn_in = artl::digital_in<artl::port::E, 0>;

artl::button<> btn;

bool led_test = false;

}

namespace ui {

void init(void)
{
    led_pwr::setup();
    led_rx0::setup();

    led_pwr::high();
    led_rx0::low();

    btn_in::setup();

    // for (;;) { }

    TCD2.INTCTRLA |= TC2_HUNFINTLVL_MED_gc; // enable HIGH underflow interrupt, pri level 2 (see 15.10.5 in AU manual)
}

void powerdown(void)
{
    TCD2.INTCTRLA &= ~TC2_HUNFINTLVL_MED_gc; // disable HIGH underflow interrupt, pri level 2 (see 15.10.5 in AU manual)

    pulse_state.low<led_pwr>();
    rst_blink_state.low<led_pwr>();

    rx_blink_state[0].low<led_rx0>();
}

void wakeup(void)
{
    TCD2.INTCTRLA |= TC2_HUNFINTLVL_MED_gc; // enable HIGH underflow interrupt, pri level 2 (see 15.10.5 in AU manual)
}

bool btn_update(unsigned long t) {
    return false;
    //return btn.update(!btn_in::read(), t);
}

bool btn_down() {
    return false;
    //return btn.down();
}

void usb_midi_disable() {
    usb_midi_enabled = false;
}

void led_test_enable() {
    if (led_test) return;

    led_test = true;

    pulse_state.high<led_pwr>();
    rst_blink_state.high<led_pwr>();

    rx_blink_state[0].high<led_rx0>();
}

void led_test_disable() {
    if (!led_test) return;

    led_test = false;

    if (!rst_blink_active) {
        pulse_state.force_write<led_pwr>();
    } else {
        rst_blink_state.force_write<led_pwr>();
    }

    rx_blink_state[0].force_write<led_rx0>();
}

void startup_animation() {
    const unsigned long frame_delay = 150;
    unsigned long t = millis();

    artl::timer<> next_frame;
    next_frame.schedule(t + frame_delay);
    uint8_t frame_no = 0;

    while (true) {
        sleepmgr_enter_sleep();

        if (!tc_flag) continue;

        tc_flag = 0;

        t = millis();

        if (!next_frame.update(t)) continue;

        ++frame_no;

        switch (frame_no) {
        case 1: rx_blink(0); break;
        case 2: rx_blink(0); break;
        case 3: tx_blink(0); break;
        case 4: tx_blink(0); break;
        case 5: rx_blink(0); break;
        case 6: rx_blink(0); break;
        case 7: /* blank frame */; break;
        }

        if (frame_no < 7) {
            next_frame.schedule(t + frame_delay);
        } else {
            break;
        }
    }

    if (!usb_midi_enabled) { usb_midi_disable(); }
    pulse_state.last_write = false;
}

}

using namespace ui;

ISR(TCD2_HUNF_vect)
{
    if (led_test) {
        if (rst_blink_active) {
            rst_blink_state.write<led_pwr>();
        }

        return;
    }

    if (!rst_blink_active) {
        pulse_state.write<led_pwr>();
    } else {
        rst_blink_state.write<led_pwr>();
    }

    rx_blink_state[0].write<led_rx0>();
}
