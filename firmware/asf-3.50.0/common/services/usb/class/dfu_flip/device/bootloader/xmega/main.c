/**
 * \file
 *
 * \brief Main functions
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
#include "conf_usb.h"

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	// Map interrupt vectors table in bootloader section
	ccp_write_io((uint8_t*)&PMIC.CTRL, PMIC_IVSEL_bm | PMIC_LOLVLEN_bm
			| PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm);

	sysclk_init();
	cpu_irq_enable();

#if defined(MIDI_ROUTER_X4)
# define ISP_LED_ON (PORTA.OUTSET = (1 << 6))
# define ISP_LED_OFF (PORTA.OUTCLR = (1 << 6))

	PORTA.DIRSET = (1 << 5) | (1 << 6) | (1 << 7);
	PORTB.DIRSET = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
	PORTC.DIRSET = (1 << 3) | (1 << 4) | (1 << 5) | (1 << 7);
	PORTD.DIRSET = (1 << 0) | (1 << 1) | (1 << 3);
	PORTE.DIRSET = (1 << 3);
#elif defined(MIDI_ROUTER_X2)
# define ISP_LED_ON (PORTD.OUTSET = (1 << 1))
# define ISP_LED_OFF (PORTD.OUTCLR = (1 << 1))

	PORTC.DIRSET = (1 << 0) | (1 << 1) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 7);
	PORTD.DIRSET = (1 << 0) | (1 << 1) | (1 << 3) | (1 << 4);
	PORTE.DIRSET = (1 << 0) | (1 << 1) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 7);
	PORTF.DIRSET = (1 << 0) | (1 << 1) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 7);

	PORTC.OUTSET = (1 << 3) | (1 << 7);
	PORTD.DIRSET = (1 << 3);
	PORTE.OUTSET = (1 << 3) | (1 << 7);
	PORTF.OUTSET = (1 << 3) | (1 << 7);
#elif defined(MIDI_ROUTER_X7)
# define ISP_LED_ON (PORTD.OUTSET = (1 << 5))
# define ISP_LED_OFF (PORTD.OUTCLR = (1 << 5))

	PORTA.DIRSET = (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);
	PORTC.DIRSET = (1 << 3) | (1 << 7);
	PORTD.DIRSET = (1 << 0) | (1 << 1) | (1 << 3) | (1 << 4) | (1 << 5);
	PORTE.DIRSET = (1 << 3) | (1 << 7);
	PORTF.DIRSET = (1 << 3) | (1 << 5) | (1 << 7);
	PORTJ.DIRSET = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7);

	PORTC.OUTSET = (1 << 3) | (1 << 7);
	PORTD.DIRSET = (1 << 3);
	PORTE.OUTSET = (1 << 3) | (1 << 7);
	PORTF.OUTSET = (1 << 3) | (1 << 7);

	PORTK.DIRSET = (1 << 1);
#else
# define ISP_LED_ON
# define ISP_LED_OFF
#endif

	// Start USB stack to authorize VBus monitoring
	udc_start();

	while (true) {
	}
}

void main_sof_action(void)
{
	static uint8_t rst_count = 0;
	uint16_t fn = udd_get_frame_number() % 256;

	switch (fn) {
	case 0:
		ISP_LED_ON;

		if ((ISP_PORT_IN & (1 << ISP_PORT_PIN)) == 0) {
			rst_count++;
			if (rst_count > 10) {
				ccp_write_io((uint8_t *)&RST.CTRL, RST_SWRST_bm);
			}
		} else {
			rst_count = 0;
		}
		break;

	case 128:
		ISP_LED_OFF;
		break;
	}
}

/**
 * \mainpage ASF USB Device DFU
 *
 * \section intro Introduction
 * This application is the official Atmel USB DFU bootloader for XMEGA.
 *
 * A USB DFU bootloader guide is proposed for all AVR XMEGA parts with USB interface:
 *
 * \section files Application Files
 * The specific files of application are:
 * - main.c: the main file to start clock and USB DFU Device.
 * - specific linker script
 * - specific optimized codes to fit with boot section size
 */
