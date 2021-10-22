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

#include "asf.h"
#include <ui.h>

#include "led.h"
#include <artl/button.h>
#include <artl/digital_in.h>
#include <timer.h>
#include <artl/timer.h>

namespace {

enum {
    MAX_PORT = 4
};

using btn_in = artl::digital_in<artl::port::C, 1>;

artl::button<> btn;

uint16_t led_rst_state = 0;
uint16_t led_rx_state[5] = {0, 0, 0, 0, 0};
uint16_t led_tx_state[5] = {0, 0, 0, 0, 0};

bool usb_midi_enabled = false;

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
    led_tx0::setup();
    led_tx1::setup();
    led_tx2::setup();
    led_tx3::setup();

    btn_in::setup();

    TCD2.INTCTRLA |= TC2_HUNFINTLVL_HI_gc; // enable HIGH underflow interrupt, pri level 3 (see 15.10.5 in AU manual)
}

void powerdown(void)
{
    TCD2.INTCTRLA &= ~TC2_HUNFINTLVL_HI_gc; // enable HIGH underflow interrupt, pri level 3 (see 15.10.5 in AU manual)

    led_pwr::low();
    led_txusb::low();
    led_rxusb::low();
    led_rx0::low();
    led_rx1::low();
    led_rx2::low();
    led_rx3::low();
    led_tx0::low();
    led_tx1::low();
    led_tx2::low();
    led_tx3::low();
}

void wakeup(void)
{
    TCD2.INTCTRLA |= TC2_HUNFINTLVL_HI_gc; // enable HIGH underflow interrupt, pri level 3 (see 15.10.5 in AU manual)
}

void com_error(void)
{
}

void com_overflow(void)
{
}

bool btn_update(unsigned long t) {
    return btn.update(!btn_in::read(), t);
}

bool btn_down() {
    return btn.down();
}

void rst_blink() {
    led_rst_state = 0x8000;
}

void rx_blink(uint8_t port) {
    if (led_rx_state[port] < 0x2000) {
        led_rx_state[port] = 0x8000;
    }
}

void tx_blink(uint8_t port) {
    if (led_tx_state[port] < 0x2000) {
        led_tx_state[port] = 0x8000;
    }
}

void rx_usb_blink() {
    rx_blink(4);
}

void tx_usb_blink() {
    tx_blink(4);
}

void tx_blink() {
    for (uint8_t i = 0; i < MAX_PORT; ++i) {
        tx_blink(i);
    }
}

void usb_midi_enable() {
    usb_midi_enabled = true;

    rx_blink(4);
    tx_blink(4);
}

void usb_midi_disable() {
    usb_midi_enabled = false;

    led_rx_state[4] = 0;
    led_rxusb::high();

    led_tx_state[4] = 0;
    led_txusb::high();
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
        case 1: rx_blink(4); tx_blink(4); break;
        case 2: rx_blink(0); tx_blink(3); break;
        case 3: rx_blink(1); tx_blink(2); break;
        case 4: rx_blink(2); tx_blink(1); break;
        case 5: rx_blink(3); tx_blink(0); break;
        case 6: /* blank frame */; break;
        }

        if (frame_no < 6) {
            next_frame.schedule(t + frame_delay);
        } else {
            break;
        }
    }

    if (!usb_midi_enabled) { usb_midi_disable(); }
}

}

namespace {

bool led_state(uint16_t &state, int16_t delta = 0) {
    uint8_t p = state >> 8;

    if (p > 254) { p = 254; }
    if (p > 127) { p = 254 - p; }

    p += p;

    bool res = ((state & 0xFF) <= p);

    state += delta;

    return res;
}

/* Breathing animation */
void LEDPulse()
{
    enum { START = 0x2000 };
    static uint16_t pulse_state = START;

    ++pulse_state;
    if (pulse_state >= 0xFFFF - START) { pulse_state = START; }

    uint16_t c;
    if (btn.down()) {
        c = led_rst_state;

        if (led_rst_state > 0) {
            led_rst_state -= 4;
        }

    } else {
        c = pulse_state;
    }

    led_pwr::write(led_state(c));

    if (led_rx_state[0]) { led_rx0::write(led_state(led_rx_state[0], -4)); }
    if (led_rx_state[1]) { led_rx1::write(led_state(led_rx_state[1], -4)); }
    if (led_rx_state[2]) { led_rx2::write(led_state(led_rx_state[2], -4)); }
    if (led_rx_state[3]) { led_rx3::write(led_state(led_rx_state[3], -4)); }
    if (led_rx_state[4]) { led_rxusb::write(led_state(led_rx_state[4], -4)); }

    if (led_tx_state[0]) { led_tx0::write(led_state(led_tx_state[0], -4)); }
    if (led_tx_state[1]) { led_tx1::write(led_state(led_tx_state[1], -4)); }
    if (led_tx_state[2]) { led_tx2::write(led_state(led_tx_state[2], -4)); }
    if (led_tx_state[3]) { led_tx3::write(led_state(led_tx_state[3], -4)); }
    if (led_tx_state[4]) { led_txusb::write(led_state(led_tx_state[4], -4)); }
}

}

ISR(TCD2_HUNF_vect)
{
    LEDPulse();
}
