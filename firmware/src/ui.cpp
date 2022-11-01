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

#include "blink_state.h"
#include "pulse_state.h"
#include "timer.h"

namespace {
bool led_test = false;
}

namespace ui {

pulse_state_t pulse_state;
blink_state_t rst_blink_state;
blink_state_t rx_blink_state[MIDI_IN_PORTS + 1];
blink_state_t tx_blink_state[MIDI_OUT_PORTS + 1];

bool usb_midi_enabled = false;

void powerdown(void)
{
    timer_disable();

    rst_blink_state.write(false);

    for(uint8_t i = 0; i <= USB_LED_ID; ++i) {
        rx_blink_state[i].write(false);
        tx_blink_state[i].write(false);
    }
}

void wakeup(void)
{
    timer_enable();
}

void usb_midi_disable() {
    usb_midi_enabled = false;

    rx_blink_state[USB_LED_ID].stop();
    rx_blink_state[USB_LED_ID].write(true);

    tx_blink_state[USB_LED_ID].stop();
    tx_blink_state[USB_LED_ID].write(true);
}

void led_test_enable() {
    if (led_test) return;

    led_test = true;

    rst_blink_state.write(true);

    for(uint8_t i = 0; i <= USB_LED_ID; ++i) {
        rx_blink_state[i].write(true);
        tx_blink_state[i].write(true);
    }
}

void led_test_disable() {
    if (!led_test) return;

    led_test = false;

    if (!btn_down()) {
        pulse_state.force_write();
    } else {
        rst_blink_state.force_write();
    }

    for(uint8_t i = 0; i < USB_LED_ID; ++i) {
        rx_blink_state[i].force_write();
        tx_blink_state[i].force_write();
    }

    if (usb_midi_enabled) {
        rx_blink_state[USB_LED_ID].force_write();
        tx_blink_state[USB_LED_ID].force_write();
    } else {
        rx_blink_state[USB_LED_ID].write(true);
        tx_blink_state[USB_LED_ID].write(true);
    }
}

void led_update() {
    if (led_test) {
        if (btn_down()) {
            rst_blink_state.write();
        }

        return;
    }

    if (!btn_down()) {
        pulse_state.write();
    } else {
        rst_blink_state.write();
    }

    for(uint8_t i = 0; i <= USB_LED_ID; ++i) {
        rx_blink_state[i].write();
        tx_blink_state[i].write();
    }
}

void generic_startup_animation() {
    const unsigned long frame_delay = 150;

    for (uint8_t frame_no = 0; frame_no <= MIDI_PORTS; ++frame_no) {
        delay(frame_delay);

        rx_blink(frame_no ? frame_no - 1 : MIDI_PORTS);
        tx_blink(MIDI_PORTS - frame_no);
    }

    delay(frame_delay); // blank frame

    if (!usb_midi_enabled) { usb_midi_disable(); }
}

}
