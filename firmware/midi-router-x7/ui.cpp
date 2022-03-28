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

using btn_in = artl::digital_in<artl::port::A, 1>;

artl::button<> btn;

bool led_test = false;

}

namespace ui {

void init(void)
{
    led_pwr::setup();
    led_txusb::setup();
    led_rxusb::setup();
    led_rx0::setup();
    led_rx1::setup();
    led_rx2::setup();
    led_rx3::setup();
    led_rx4::setup();
    led_rx5::setup();
    led_rx6::setup();
    led_tx0::setup();
    led_tx1::setup();
    led_tx2::setup();
    led_tx3::setup();
    led_tx4::setup();
    led_tx5::setup();
    led_tx6::setup();

    oen::setup();

    btn_in::setup();

    TCD2.INTCTRLA |= TC2_HUNFINTLVL_MED_gc; // enable HIGH underflow interrupt, pri level 2 (see 15.10.5 in AU manual)
}

void powerdown(void)
{
    TCD2.INTCTRLA &= ~TC2_HUNFINTLVL_MED_gc; // disable HIGH underflow interrupt, pri level 2 (see 15.10.5 in AU manual)

    led_pwr::low();
    led_txusb::low();
    led_rxusb::low();

    led_rx0::low();
    led_rx1::low();
    led_rx2::low();
    led_rx3::low();
    led_rx4::low();
    led_rx5::low();
    led_rx6::low();

    led_tx0::low();
    led_tx1::low();
    led_tx2::low();
    led_tx3::low();
    led_tx4::low();
    led_tx5::low();
    led_tx6::low();
}

void wakeup(void)
{
    TCD2.INTCTRLA |= TC2_HUNFINTLVL_MED_gc; // enable HIGH underflow interrupt, pri level 2 (see 15.10.5 in AU manual)
}

bool btn_update(unsigned long t) {
    return btn.update(!btn_in::read(), t);
}

bool btn_down() {
    return btn.down();
}

void usb_midi_disable() {
    usb_midi_enabled = false;

    rx_blink_state[USB_LED_ID].stop();
    led_rxusb::high();

    tx_blink_state[USB_LED_ID].stop();
    led_txusb::high();
}

void led_test_enable() {
    if (led_test) return;

    led_test = true;

    led_pwr::high();
    led_txusb::high();
    led_rxusb::high();

    led_rx0::high();
    led_rx1::high();
    led_rx2::high();
    led_rx3::high();
    led_rx4::high();
    led_rx5::high();
    led_rx6::high();

    led_tx0::high();
    led_tx1::high();
    led_tx2::high();
    led_tx3::high();
    led_tx4::high();
    led_tx5::high();
    led_tx6::high();
}

void led_test_disable() {
    if (!led_test) return;

    led_test = false;

    if (!btn.down()) {
        pulse_state.force_write<led_pwr>();
    } else {
        rst_blink_state.force_write<led_pwr>();
    }

    rx_blink_state[0].force_write<led_rx0>();
    rx_blink_state[1].force_write<led_rx1>();
    rx_blink_state[2].force_write<led_rx2>();
    rx_blink_state[3].force_write<led_rx3>();
    rx_blink_state[4].force_write<led_rx4>();
    rx_blink_state[5].force_write<led_rx5>();
    rx_blink_state[6].force_write<led_rx6>();

    tx_blink_state[0].force_write<led_tx0>();
    tx_blink_state[1].force_write<led_tx1>();
    tx_blink_state[2].force_write<led_tx2>();
    tx_blink_state[3].force_write<led_tx3>();
    tx_blink_state[4].force_write<led_tx4>();
    tx_blink_state[5].force_write<led_tx5>();
    tx_blink_state[6].force_write<led_tx6>();

    if (usb_midi_enabled) {
        rx_blink_state[USB_LED_ID].force_write<led_rxusb>();
        tx_blink_state[USB_LED_ID].force_write<led_txusb>();
    } else {
        led_rxusb::high();
        led_txusb::high();
    }
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
        case 1: rx_blink(7); tx_blink(7); break;
        case 2: rx_blink(0); tx_blink(6); break;
        case 3: rx_blink(1); tx_blink(5); break;
        case 4: rx_blink(2); tx_blink(4); break;
        case 5: rx_blink(3); tx_blink(3); break;
        case 6: rx_blink(4); tx_blink(2); break;
        case 7: rx_blink(5); tx_blink(1); break;
        case 8: rx_blink(6); tx_blink(0); break;
        case 9: /* blank frame */; break;
        }

        if (frame_no < 9) {
            next_frame.schedule(t + frame_delay);
        } else {
            break;
        }
    }

    if (!usb_midi_enabled) { usb_midi_disable(); }
}

}

using namespace ui;

ISR(TCD2_HUNF_vect)
{
    if (led_test) {
        if (btn.down()) {
            rst_blink_state.write<led_pwr>();
        }

        return;
    }

    if (!btn.down()) {
        pulse_state.write<led_pwr>();
    } else {
        rst_blink_state.write<led_pwr>();
    }

    rx_blink_state[0].write<led_rx0>();
    rx_blink_state[1].write<led_rx1>();
    rx_blink_state[2].write<led_rx2>();
    rx_blink_state[3].write<led_rx3>();
    rx_blink_state[4].write<led_rx4>();
    rx_blink_state[5].write<led_rx5>();
    rx_blink_state[6].write<led_rx6>();
    rx_blink_state[7].write<led_rxusb>();

    tx_blink_state[0].write<led_tx0>();
    tx_blink_state[1].write<led_tx1>();
    tx_blink_state[2].write<led_tx2>();
    tx_blink_state[3].write<led_tx3>();
    tx_blink_state[4].write<led_tx4>();
    tx_blink_state[5].write<led_tx5>();
    tx_blink_state[6].write<led_tx6>();
    tx_blink_state[7].write<led_txusb>();
}
