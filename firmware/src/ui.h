/**
 * \file
 *
 * \brief Common User Interface for USB application
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

#pragma once

extern "C" {
#include "asf.h"
}

#include "blink_state.h"
#include "pulse_state.h"

#define USB_LED_ID MIDI_PORTS

namespace ui {

extern pulse_state_t pulse_state;
extern blink_state_t rst_blink_state;
extern blink_state_t rx_blink_state[MIDI_PORTS + 1];
extern blink_state_t tx_blink_state[MIDI_PORTS + 1];

extern bool usb_midi_enabled;

//! \brief Initializes the user interface
void init(void);

//! \brief Enters the user interface in power down mode
void powerdown(void);

//! \brief Exits the user interface of power down mode
void wakeup(void);

bool btn_update(unsigned long t);
bool btn_down();

inline void rst_blink() { rst_blink_state.start(); }

inline void rx_blink(uint8_t port) { rx_blink_state[port].start(); }
inline void tx_blink(uint8_t port) { tx_blink_state[port].start(); }

inline void rx_active(uint8_t port) { rx_blink_state[port].active(); }
inline void tx_active(uint8_t port) { tx_blink_state[port].active(); }

inline void rx_usb_blink() { rx_blink_state[USB_LED_ID].start(); }
inline void tx_usb_blink() { tx_blink_state[USB_LED_ID].start(); }

inline void rx_usb_active() { rx_blink_state[USB_LED_ID].active(); }
inline void tx_usb_active() { tx_blink_state[USB_LED_ID].active(); }

inline void tx_blink() {
    for (uint8_t i = 0; i < MIDI_PORTS; ++i) {
        tx_blink_state[i].start();
    }
}

inline void usb_midi_enable() {
    usb_midi_enabled = true;

    rx_blink_state[USB_LED_ID].start();
    tx_blink_state[USB_LED_ID].start();
}

void usb_midi_disable();

void startup_animation();

}
