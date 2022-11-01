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

namespace {

using btn_in = artl::digital_in<artl::port::A, 1>;

artl::button<> btn;

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

    rx_blink_state[0].setup<led_rx0>();
    rx_blink_state[1].setup<led_rx1>();
    rx_blink_state[2].setup<led_rx2>();
    rx_blink_state[3].setup<led_rx3>();
    rx_blink_state[4].setup<led_rx4>();
    rx_blink_state[5].setup<led_rx5>();
    rx_blink_state[6].setup<led_rx6>();
    rx_blink_state[USB_LED_ID].setup<led_rxusb>();

    tx_blink_state[0].setup<led_tx0>();
    tx_blink_state[1].setup<led_tx1>();
    tx_blink_state[2].setup<led_tx2>();
    tx_blink_state[3].setup<led_tx3>();
    tx_blink_state[4].setup<led_tx4>();
    tx_blink_state[5].setup<led_tx5>();
    tx_blink_state[6].setup<led_tx6>();
    tx_blink_state[USB_LED_ID].setup<led_txusb>();

    pulse_state.setup<led_pwr>();
    rst_blink_state.setup<led_pwr>();

    timer_enable();
}

void timer_enable() {
    TCD2.INTCTRLA |= TC2_HUNFINTLVL_MED_gc; // enable HIGH underflow interrupt, pri level 2 (see 15.10.5 in AU manual)
}

void timer_disable() {
    TCD2.INTCTRLA &= ~TC2_HUNFINTLVL_MED_gc; // disable HIGH underflow interrupt, pri level 2 (see 15.10.5 in AU manual)
}

bool btn_update(unsigned long t) {
    return btn.update(!btn_in::read(), t);
}

bool btn_down() {
    return btn.down();
}

void startup_animation() {
    generic_startup_animation();
}

}

ISR(TCD2_HUNF_vect)
{
    ui::led_update();
}
