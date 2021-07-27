/**
 * \file
 *
 * \brief USB configuration file
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

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"

/**
 * USB Device Configuration
 * @{
 */

/* See https://raw.githubusercontent.com/arduino/ArduinoISP/master/usbdrv/USB-IDs-for-free.txt */

//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID             0x16c0
#define  USB_DEVICE_PRODUCT_ID            0x05e4
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 100 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR                  (USB_CONFIG_ATTR_BUS_POWERED)
//	(USB_CONFIG_ATTR_SELF_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
#define  USB_DEVICE_MANUFACTURE_NAME      "MIDI-Lab.ru"
#define  USB_DEVICE_PRODUCT_NAME          "MIDI Router x4"
#define  USB_DEVICE_GET_SERIAL_NAME_POINTER  main_serial_name()
#define  USB_DEVICE_GET_SERIAL_NAME_LENGTH   16 /* 2 * sizeof(struct nvm_device_serial) */

/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)
#define  UDC_SOF_EVENT()                  main_sof_action()
#define  UDC_SUSPEND_EVENT()              main_suspend_action()
#define  UDC_RESUME_EVENT()               main_resume_action()
//! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
// #define  UDC_REMOTEWAKEUP_ENABLE()        user_callback_remotewakeup_enable()
// extern void user_callback_remotewakeup_enable(void);
// #define  UDC_REMOTEWAKEUP_DISABLE()       user_callback_remotewakeup_disable()
// extern void user_callback_remotewakeup_disable(void);
//! When a extra string descriptor must be supported
//! other than manufacturer, product and serial string
//# d efine  UDC_GET_EXTRA_STRING()           main_extra_string()
//@}

/**
 * USB Device low level configuration
 * When only one interface is used, these configurations are defined by the class module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Control endpoint size
#define  USB_DEVICE_EP_CTRL_SIZE       64

//! Four interfaces for this device (CDC COM + CDC DATA + AUDIO CONTROL + MIDI STREAMING)
#define  USB_DEVICE_NB_INTERFACE       4

//! 5 endpoints used by CDC and MIDI interfaces
// (1 | USB_EP_DIR_IN)  // CDC Notify endpoint
// (2 | USB_EP_DIR_IN)  // CDC TX
// (3 | USB_EP_DIR_OUT) // CDC RX
// (4 | USB_EP_DIR_OUT) // MIDI OUT
// (5 | USB_EP_DIR_IN)  // MIDI IN
#define  USB_DEVICE_MAX_EP             5
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */
/**
 * Configuration of CDC interface
 * @{
 */

//! Define USB communication port
#define  UDI_CDC_PORT_NB                  1

//! Interface callback definition
#define  UDI_CDC_ENABLE_EXT(port)         main_cdc_enable(port)
#define  UDI_CDC_DISABLE_EXT(port)        main_cdc_disable(port)
#define  UDI_CDC_RX_NOTIFY(port)          main_cdc_rx_notify(port)
#define  UDI_CDC_TX_EMPTY_NOTIFY(port)
#define  UDI_CDC_SET_CODING_EXT(port,cfg) main_cdc_config(port,cfg)
#define  UDI_CDC_SET_DTR_EXT(port,set)    main_cdc_set_dtr(port,set)
#define  UDI_CDC_SET_RTS_EXT(port,set)

//! Define it when the transfer CDC Device to Host is a low rate (<512000 bauds)
//! to reduce CDC buffers size
//#define  UDI_CDC_LOW_RATE

//! Default configuration of communication port
#define  UDI_CDC_DEFAULT_RATE             115200
#define  UDI_CDC_DEFAULT_STOPBITS         CDC_STOP_BITS_1
#define  UDI_CDC_DEFAULT_PARITY           CDC_PAR_NONE
#define  UDI_CDC_DEFAULT_DATABITS         8

//! Enable id string of interface to add an extra USB string
//#define  UDI_CDC_IAD_STRING_ID            4

/**
 * USB CDC low level configuration
 * In standalone these configurations are defined by the CDC module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_CDC_COMM_EP_0             (1 | USB_EP_DIR_IN) // Notify endpoint
#define  UDI_CDC_DATA_EP_IN_0          (2 | USB_EP_DIR_IN) // TX
#define  UDI_CDC_DATA_EP_OUT_0         (3 | USB_EP_DIR_OUT)// RX

//! Interface numbers
#define  UDI_CDC_COMM_IFACE_NUMBER_0   0
#define  UDI_CDC_DATA_IFACE_NUMBER_0   1
//@}
//@}


/**
 * Configuration of MIDI interface
 * @{
 */

/**
 * USB MIDI low level configuration
 * In standalone these configurations are defined by the MIDI module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_MIDI_EP_OUT                (4 | USB_EP_DIR_OUT)
#define  UDI_MIDI_EP_IN                 (5 | USB_EP_DIR_IN)

//! Interface number
#define  UDI_AUDIO_IFACE_NUMBER         2
#define  UDI_MIDI_IFACE_NUMBER          3
//@}
//@}

//@}

/**
 * Description of Composite Device
 * @{
 */
//! USB Interfaces descriptor structure
#define UDI_COMPOSITE_DESC_T \
	usb_iad_desc_t         udi_cdc_iad; \
	udi_cdc_comm_desc_t    udi_cdc_comm; \
	udi_cdc_data_desc_t    udi_cdc_data; \
	usb_iad_desc_t         udi_midi_iad;  \
	udi_audio_ctrl_desc_t  udi_audio_ctrl; \
	udi_midi_desc4_t       udi_midi

//! USB Interfaces descriptor value for Full Speed
#define UDI_COMPOSITE_DESC_FS \
	.udi_cdc_iad    = UDI_CDC_IAD_DESC_0, \
	.udi_cdc_comm   = UDI_CDC_COMM_DESC_0, \
	.udi_cdc_data   = UDI_CDC_DATA_DESC_0_FS, \
	.udi_midi_iad   = UDI_MIDI_IAD_DESC,  \
	.udi_audio_ctrl = UDI_AUDIO_CTRL_DESC, \
	.udi_midi       = UDI_MIDI_DESC4

//! USB Interfaces descriptor value for High Speed
#define UDI_COMPOSITE_DESC_HS \
	.udi_cdc_iad    = UDI_CDC_IAD_DESC_0, \
	.udi_cdc_comm   = UDI_CDC_COMM_DESC_0, \
	.udi_cdc_data   = UDI_CDC_DATA_DESC_0_HS, \
	.udi_midi_iad   = UDI_MIDI_IAD_DESC, \
	.udi_audio_ctrl = UDI_AUDIO_CTRL_DESC, \
	.udi_midi       = UDI_MIDI_DESC4

//! USB Interface APIs
#define UDI_COMPOSITE_API \
	&udi_api_cdc_comm, \
	&udi_api_cdc_data, \
	&udi_api_audio_ctrl, \
	&udi_api_midi

//	&udi_api_msc
//@}


/**
 * USB Device Driver Configuration
 * @{
 */
//@}

//! The includes of classes and other headers must be done at the end of this file to avoid compile error
#include "udi_cdc.h"
#include "udi_midi.h"
#include "main.h"

#endif // _CONF_USB_H_
