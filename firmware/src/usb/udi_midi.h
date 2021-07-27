#pragma once

#include "conf_usb.h"
#include "usb_protocol_midi.h"
#include "udc.h"
#include "udi.h"

//! Interface descriptor structure for MIDI
typedef struct {
  usb_iface_desc_t iface;
  usb_audio_ctrl_desc_t acifc;
} udi_audio_ctrl_desc_t;

typedef struct {
  usb_iface_desc_t iface;
  usb_midi_stream_desc_t msifc;
  usb_midi_jack_in_desc_t jin_int;
  usb_midi_jack_in_desc_t jin_ext;
  usb_midi_jack_out_desc_t jout_int;
  usb_midi_jack_out_desc_t jout_ext;
  usb_midi_jack_ep_desc_t ep_out;
  usb_midi_jack_ep_ac1_desc_t ep_ac_out;
  usb_midi_jack_ep_desc_t ep_in;
  usb_midi_jack_ep_ac1_desc_t ep_ac_in;
} udi_midi_desc1_t;

typedef struct {
  usb_iface_desc_t iface;
  usb_midi_stream_desc_t msifc;
  usb_midi_jack_in_desc_t jin_int[2];
  usb_midi_jack_in_desc_t jin_ext[2];
  usb_midi_jack_out_desc_t jout_int[2];
  usb_midi_jack_out_desc_t jout_ext[2];
  usb_midi_jack_ep_desc_t ep_out;
  usb_midi_jack_ep_ac2_desc_t ep_ac_out;
  usb_midi_jack_ep_desc_t ep_in;
  usb_midi_jack_ep_ac2_desc_t ep_ac_in;
} udi_midi_desc2_t;

typedef struct {
  usb_iface_desc_t iface;
  usb_midi_stream_desc_t msifc;
  usb_midi_jack_in_desc_t jin_int[4];
  usb_midi_jack_in_desc_t jin_ext[4];
  usb_midi_jack_out_desc_t jout_int[4];
  usb_midi_jack_out_desc_t jout_ext[4];
  usb_midi_jack_ep_desc_t ep_out;
  usb_midi_jack_ep_ac4_desc_t ep_ac_out;
  usb_midi_jack_ep_desc_t ep_in;
  usb_midi_jack_ep_ac4_desc_t ep_ac_in;
} udi_midi_desc4_t;

typedef struct {
  usb_iface_desc_t iface;
  usb_midi_stream_desc_t msifc;
  usb_midi_jack_in_desc_t jin_int[5];
  usb_midi_jack_in_desc_t jin_ext[5];
  usb_midi_jack_out_desc_t jout_int[5];
  usb_midi_jack_out_desc_t jout_ext[5];
  usb_midi_jack_ep_desc_t ep_out;
  usb_midi_jack_ep_ac5_desc_t ep_ac_out;
  usb_midi_jack_ep_desc_t ep_in;
  usb_midi_jack_ep_ac5_desc_t ep_ac_in;
} udi_midi_desc5_t;

typedef struct {
  usb_iface_desc_t iface;
  usb_midi_stream_desc_t msifc;
  usb_midi_jack_in_desc_t jin_int[6];
  usb_midi_jack_in_desc_t jin_ext[6];
  usb_midi_jack_out_desc_t jout_int[6];
  usb_midi_jack_out_desc_t jout_ext[6];
  usb_midi_jack_ep_desc_t ep_out;
  usb_midi_jack_ep_ac6_desc_t ep_ac_out;
  usb_midi_jack_ep_desc_t ep_in;
  usb_midi_jack_ep_ac6_desc_t ep_ac_in;
} udi_midi_desc6_t;

typedef struct {
  usb_iface_desc_t iface;
  usb_midi_stream_desc_t msifc;
  usb_midi_jack_in_desc_t jin_int[7];
  usb_midi_jack_in_desc_t jin_ext[7];
  usb_midi_jack_out_desc_t jout_int[7];
  usb_midi_jack_out_desc_t jout_ext[7];
  usb_midi_jack_ep_desc_t ep_out;
  usb_midi_jack_ep_ac7_desc_t ep_ac_out;
  usb_midi_jack_ep_desc_t ep_in;
  usb_midi_jack_ep_ac7_desc_t ep_ac_in;
} udi_midi_desc7_t;

//! Content of MIDI IAD interface descriptor for all speeds
#define UDI_MIDI_IAD_DESC { \
   .bLength                      = sizeof(usb_iad_desc_t),\
   .bDescriptorType              = USB_DT_IAD,\
   .bInterfaceCount              = 2,\
   .bFunctionClass               = AUDIO_CLASS_CODE, \
   .bFunctionSubClass            = SUBCLASS_AUDIOCONTROL, \
   .bFunctionProtocol            = 0,\
   .bFirstInterface              = UDI_AUDIO_IFACE_NUMBER,\
   .iFunction                    = 0,\
   }

#define UDI_AUDIO_CTRL_DESC { \
   .iface.bLength                = sizeof(usb_iface_desc_t),\
   .iface.bDescriptorType        = USB_DT_INTERFACE,\
   .iface.bAlternateSetting      = 0,\
   .iface.bNumEndpoints          = 0,\
   .iface.bInterfaceClass        = AUDIO_CLASS_CODE,\
   .iface.bInterfaceSubClass     = SUBCLASS_AUDIOCONTROL,\
   .iface.bInterfaceProtocol     = 0,\
   .iface.bInterfaceNumber       = UDI_AUDIO_IFACE_NUMBER,\
   .iface.iInterface             = 0,\
   .acifc.bLength                = sizeof(usb_audio_ctrl_desc_t), \
   .acifc.bDescriptorType        = AUDIO_CS_INTERFACE, /* 0x24 */ \
   .acifc.bDescriptorSubtype     = AC_HEADER, /* 0x01 */ \
   .acifc.bcdADc                 = LE16(0x0100), \
   .acifc.wTotalLength           = LE16(sizeof(usb_audio_ctrl_desc_t)), /* 9 */ \
   .acifc.bInCollection          = 1, \
   .acifc.interfaceNumbers       = UDI_MIDI_IFACE_NUMBER, \
   }

// JACK_TYPE_EMBEDDED or JACK_TYPE_EXTERNAL
#define USB_MIDI_IN_JACK(type, id) { \
   .bLength              = sizeof(usb_midi_jack_in_desc_t), \
   .bDescriptorType      = MIDI_CS_INTERFACE, /* 0x24 */ \
   .bDescriptorSubtype   = MS_MIDI_IN_JACK, /* 0x02 */ \
   .jackType             = (type), \
   .jackID               = (id), \
   .jackStrIndex         = 0, \
   }

#define USB_MIDI_OUT_JACK(type, id, src) { \
   .bLength             = sizeof(usb_midi_jack_out_desc_t), \
   .bDescriptorType     = MIDI_CS_INTERFACE, /* 0x24 */ \
   .bDescriptorSubtype  = MS_MIDI_OUT_JACK, /* 0x03 */ \
   .jackType            = (type), \
   .jackID              = (id), \
   .nPins               = 1, \
   .srcJackID           = (src), \
   .srcPinID            = 1, \
   .jackStrIndex        = 0, \
   }

#define UDI_MIDI_IFACE_DESC { \
   .bLength                = sizeof(usb_iface_desc_t),\
   .bDescriptorType        = USB_DT_INTERFACE,\
   .bAlternateSetting      = 0,\
   .bNumEndpoints          = 2,\
   .bInterfaceClass        = AUDIO_CLASS_CODE,\
   .bInterfaceSubClass     = SUBCLASS_MIDISTREAMING,\
   .bInterfaceProtocol     = 0,\
   .bInterfaceNumber       = UDI_MIDI_IFACE_NUMBER,\
   .iInterface             = 0,\
    }

#define UDI_MIDI_DESC1 { \
   .iface                        = UDI_MIDI_IFACE_DESC, \
   .msifc.bLength                = sizeof(usb_midi_stream_desc_t), \
   .msifc.bDescriptorType        = MIDI_CS_INTERFACE, /* 0x24 */ \
   .msifc.bDescriptorSubtype     = AC_HEADER, /* 0x01 */ \
   .msifc.bcdADc                 = LE16(0x0100), \
   .msifc.wTotalLength           = LE16(sizeof(udi_midi_desc1_t) - sizeof(usb_iface_desc_t)), \
   .jin_int                      = USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 1), \
   .jin_ext                      = USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 2), \
   .jout_int                     = USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 3, 2), \
   .jout_ext                     = USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 4, 1), \
   .ep_out.ep.bLength            = sizeof(usb_midi_jack_ep_desc_t),\
   .ep_out.ep.bDescriptorType    = USB_DT_ENDPOINT,\
   .ep_out.ep.bmAttributes       = USB_EP_TYPE_BULK,\
   .ep_out.ep.wMaxPacketSize     = LE16(64),\
   .ep_out.ep.bInterval          = 0,\
   .ep_out.ep.bEndpointAddress   = UDI_MIDI_EP_OUT,\
   .ep_out.refresh               = 0,\
   .ep_out.sync                  = 0,\
   .ep_ac_out.bLength            = sizeof(usb_midi_jack_ep_ac1_desc_t), \
   .ep_ac_out.bDescriptorType    = MIDI_CS_ENDPOINT, /* 0x25 */ \
   .ep_ac_out.bDescriptorSubtype = MS_GENERAL, /* 0x01 */ \
   .ep_ac_out.embJacks           = 1, \
   .ep_ac_out.jackID             = 1, \
   .ep_in.ep.bLength             = sizeof(usb_midi_jack_ep_desc_t),\
   .ep_in.ep.bDescriptorType     = USB_DT_ENDPOINT,\
   .ep_in.ep.bmAttributes        = USB_EP_TYPE_BULK,\
   .ep_in.ep.wMaxPacketSize      = LE16(64),\
   .ep_in.ep.bInterval           = 0,\
   .ep_in.ep.bEndpointAddress    = UDI_MIDI_EP_IN,\
   .ep_in.refresh                = 0,\
   .ep_in.sync                   = 0,\
   .ep_ac_in.bLength             = sizeof(usb_midi_jack_ep_ac1_desc_t), \
   .ep_ac_in.bDescriptorType     = MIDI_CS_ENDPOINT, /* 0x25 */ \
   .ep_ac_in.bDescriptorSubtype  = MS_GENERAL, /* 0x01 */ \
   .ep_ac_in.embJacks            = 1, \
   .ep_ac_in.jackID              = 3, \
   }

#define UDI_MIDI_DESC2 { \
   .iface                        = UDI_MIDI_IFACE_DESC, \
   .msifc.bLength                = sizeof(usb_midi_stream_desc_t), \
   .msifc.bDescriptorType        = MIDI_CS_INTERFACE, /* 0x24 */ \
   .msifc.bDescriptorSubtype     = AC_HEADER, /* 0x01 */ \
   .msifc.bcdADc                 = LE16(0x0100), \
   .msifc.wTotalLength           = LE16(sizeof(udi_midi_desc2_t) - sizeof(usb_iface_desc_t)), \
   .jin_int                      = { \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 1), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 2), \
   }, \
   .jin_ext                      = { \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 11), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 12), \
   }, \
   .jout_int                     = { \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 21, 11), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 22, 12), \
   }, \
   .jout_ext                     = { \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 31, 1), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 32, 2), \
   }, \
   .ep_out.ep.bLength            = sizeof(usb_midi_jack_ep_desc_t),\
   .ep_out.ep.bDescriptorType    = USB_DT_ENDPOINT,\
   .ep_out.ep.bmAttributes       = USB_EP_TYPE_BULK,\
   .ep_out.ep.wMaxPacketSize     = LE16(64),\
   .ep_out.ep.bInterval          = 0,\
   .ep_out.ep.bEndpointAddress   = UDI_MIDI_EP_OUT,\
   .ep_out.refresh               = 0,\
   .ep_out.sync                  = 0,\
   .ep_ac_out.bLength            = sizeof(usb_midi_jack_ep_ac2_desc_t), \
   .ep_ac_out.bDescriptorType    = MIDI_CS_ENDPOINT, /* 0x25 */ \
   .ep_ac_out.bDescriptorSubtype = MS_GENERAL, /* 0x01 */ \
   .ep_ac_out.embJacks           = 2, \
   .ep_ac_out.jackID             = { 1, 2 }, \
   .ep_in.ep.bLength             = sizeof(usb_midi_jack_ep_desc_t),\
   .ep_in.ep.bDescriptorType     = USB_DT_ENDPOINT,\
   .ep_in.ep.bmAttributes        = USB_EP_TYPE_BULK,\
   .ep_in.ep.wMaxPacketSize      = LE16(64),\
   .ep_in.ep.bInterval           = 0,\
   .ep_in.ep.bEndpointAddress    = UDI_MIDI_EP_IN,\
   .ep_in.refresh                = 0,\
   .ep_in.sync                   = 0,\
   .ep_ac_in.bLength             = sizeof(usb_midi_jack_ep_ac2_desc_t), \
   .ep_ac_in.bDescriptorType     = MIDI_CS_ENDPOINT, /* 0x25 */ \
   .ep_ac_in.bDescriptorSubtype  = MS_GENERAL, /* 0x01 */ \
   .ep_ac_in.embJacks            = 2, \
   .ep_ac_in.jackID              = { 21, 22 }, \
   }

#define UDI_MIDI_DESC4 { \
   .iface                        = UDI_MIDI_IFACE_DESC, \
   .msifc.bLength                = sizeof(usb_midi_stream_desc_t), \
   .msifc.bDescriptorType        = MIDI_CS_INTERFACE, /* 0x24 */ \
   .msifc.bDescriptorSubtype     = AC_HEADER, /* 0x01 */ \
   .msifc.bcdADc                 = LE16(0x0100), \
   .msifc.wTotalLength           = LE16(sizeof(udi_midi_desc4_t) - sizeof(usb_iface_desc_t)), \
   .jin_int                      = { \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 1), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 2), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 3), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 4), \
   }, \
   .jin_ext                      = { \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 11), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 12), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 13), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 14), \
   }, \
   .jout_int                     = { \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 21, 11), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 22, 12), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 23, 12), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 24, 14), \
   }, \
   .jout_ext                     = { \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 31, 1), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 32, 2), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 33, 3), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 34, 4), \
   }, \
   .ep_out.ep.bLength            = sizeof(usb_midi_jack_ep_desc_t),\
   .ep_out.ep.bDescriptorType    = USB_DT_ENDPOINT,\
   .ep_out.ep.bmAttributes       = USB_EP_TYPE_BULK,\
   .ep_out.ep.wMaxPacketSize     = LE16(64),\
   .ep_out.ep.bInterval          = 0,\
   .ep_out.ep.bEndpointAddress   = UDI_MIDI_EP_OUT,\
   .ep_out.refresh               = 0,\
   .ep_out.sync                  = 0,\
   .ep_ac_out.bLength            = sizeof(usb_midi_jack_ep_ac4_desc_t), \
   .ep_ac_out.bDescriptorType    = MIDI_CS_ENDPOINT, /* 0x25 */ \
   .ep_ac_out.bDescriptorSubtype = MS_GENERAL, /* 0x01 */ \
   .ep_ac_out.embJacks           = 4, \
   .ep_ac_out.jackID             = { 1, 2, 3, 4 }, \
   .ep_in.ep.bLength             = sizeof(usb_midi_jack_ep_desc_t),\
   .ep_in.ep.bDescriptorType     = USB_DT_ENDPOINT,\
   .ep_in.ep.bmAttributes        = USB_EP_TYPE_BULK,\
   .ep_in.ep.wMaxPacketSize      = LE16(64),\
   .ep_in.ep.bInterval           = 0,\
   .ep_in.ep.bEndpointAddress    = UDI_MIDI_EP_IN,\
   .ep_in.refresh                = 0,\
   .ep_in.sync                   = 0,\
   .ep_ac_in.bLength             = sizeof(usb_midi_jack_ep_ac4_desc_t), \
   .ep_ac_in.bDescriptorType     = MIDI_CS_ENDPOINT, /* 0x25 */ \
   .ep_ac_in.bDescriptorSubtype  = MS_GENERAL, /* 0x01 */ \
   .ep_ac_in.embJacks            = 4, \
   .ep_ac_in.jackID              = { 21, 22, 23, 24 }, \
   }

#define UDI_MIDI_DESC5 { \
   .iface                        = UDI_MIDI_IFACE_DESC, \
   .msifc.bLength                = sizeof(usb_midi_stream_desc_t), \
   .msifc.bDescriptorType        = MIDI_CS_INTERFACE, /* 0x24 */ \
   .msifc.bDescriptorSubtype     = AC_HEADER, /* 0x01 */ \
   .msifc.bcdADc                 = LE16(0x0100), \
   .msifc.wTotalLength           = LE16(sizeof(udi_midi_desc5_t) - sizeof(usb_iface_desc_t)), \
   .jin_int                      = { \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 1), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 2), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 3), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 4), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 5), \
   }, \
   .jin_ext                      = { \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 11), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 12), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 13), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 14), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 15), \
   }, \
   .jout_int                     = { \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 21, 11), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 22, 12), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 23, 13), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 24, 14), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 25, 15), \
   }, \
   .jout_ext                     = { \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 31, 1), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 32, 2), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 33, 3), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 34, 4), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 35, 5), \
   }, \
   .ep_out.ep.bLength            = sizeof(usb_midi_jack_ep_desc_t),\
   .ep_out.ep.bDescriptorType    = USB_DT_ENDPOINT,\
   .ep_out.ep.bmAttributes       = USB_EP_TYPE_BULK,\
   .ep_out.ep.wMaxPacketSize     = LE16(64),\
   .ep_out.ep.bInterval          = 0,\
   .ep_out.ep.bEndpointAddress   = UDI_MIDI_EP_OUT,\
   .ep_out.refresh               = 0,\
   .ep_out.sync                  = 0,\
   .ep_ac_out.bLength            = sizeof(usb_midi_jack_ep_ac5_desc_t), \
   .ep_ac_out.bDescriptorType    = MIDI_CS_ENDPOINT, /* 0x25 */ \
   .ep_ac_out.bDescriptorSubtype = MS_GENERAL, /* 0x01 */ \
   .ep_ac_out.embJacks           = 5, \
   .ep_ac_out.jackID             = { 1, 2, 3, 4, 5 }, \
   .ep_in.ep.bLength             = sizeof(usb_midi_jack_ep_desc_t),\
   .ep_in.ep.bDescriptorType     = USB_DT_ENDPOINT,\
   .ep_in.ep.bmAttributes        = USB_EP_TYPE_BULK,\
   .ep_in.ep.wMaxPacketSize      = LE16(64),\
   .ep_in.ep.bInterval           = 0,\
   .ep_in.ep.bEndpointAddress    = UDI_MIDI_EP_IN,\
   .ep_in.refresh                = 0,\
   .ep_in.sync                   = 0,\
   .ep_ac_in.bLength             = sizeof(usb_midi_jack_ep_ac5_desc_t), \
   .ep_ac_in.bDescriptorType     = MIDI_CS_ENDPOINT, /* 0x25 */ \
   .ep_ac_in.bDescriptorSubtype  = MS_GENERAL, /* 0x01 */ \
   .ep_ac_in.embJacks            = 5, \
   .ep_ac_in.jackID              = { 21, 22, 23, 24, 25 }, \
   }

#define UDI_MIDI_DESC6 { \
   .iface                        = UDI_MIDI_IFACE_DESC, \
   .msifc.bLength                = sizeof(usb_midi_stream_desc_t), \
   .msifc.bDescriptorType        = MIDI_CS_INTERFACE, /* 0x24 */ \
   .msifc.bDescriptorSubtype     = AC_HEADER, /* 0x01 */ \
   .msifc.bcdADc                 = LE16(0x0100), \
   .msifc.wTotalLength           = LE16(sizeof(udi_midi_desc6_t) - sizeof(usb_iface_desc_t)), \
   .jin_int                      = { \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 1), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 2), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 3), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 4), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 5), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 6), \
   }, \
   .jin_ext                      = { \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 11), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 12), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 13), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 14), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 15), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 16), \
   }, \
   .jout_int                     = { \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 21, 11), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 22, 12), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 23, 13), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 24, 14), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 25, 15), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 26, 16), \
   }, \
   .jout_ext                     = { \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 31, 1), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 32, 2), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 33, 3), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 34, 4), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 35, 5), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 36, 6), \
   }, \
   .ep_out.ep.bLength            = sizeof(usb_midi_jack_ep_desc_t),\
   .ep_out.ep.bDescriptorType    = USB_DT_ENDPOINT,\
   .ep_out.ep.bmAttributes       = USB_EP_TYPE_BULK,\
   .ep_out.ep.wMaxPacketSize     = LE16(64),\
   .ep_out.ep.bInterval          = 0,\
   .ep_out.ep.bEndpointAddress   = UDI_MIDI_EP_OUT,\
   .ep_out.refresh               = 0,\
   .ep_out.sync                  = 0,\
   .ep_ac_out.bLength            = sizeof(usb_midi_jack_ep_ac6_desc_t), \
   .ep_ac_out.bDescriptorType    = MIDI_CS_ENDPOINT, /* 0x25 */ \
   .ep_ac_out.bDescriptorSubtype = MS_GENERAL, /* 0x01 */ \
   .ep_ac_out.embJacks           = 6, \
   .ep_ac_out.jackID             = { 1, 2, 3, 4, 5, 6 }, \
   .ep_in.ep.bLength             = sizeof(usb_midi_jack_ep_desc_t),\
   .ep_in.ep.bDescriptorType     = USB_DT_ENDPOINT,\
   .ep_in.ep.bmAttributes        = USB_EP_TYPE_BULK,\
   .ep_in.ep.wMaxPacketSize      = LE16(64),\
   .ep_in.ep.bInterval           = 0,\
   .ep_in.ep.bEndpointAddress    = UDI_MIDI_EP_IN,\
   .ep_in.refresh                = 0,\
   .ep_in.sync                   = 0,\
   .ep_ac_in.bLength             = sizeof(usb_midi_jack_ep_ac6_desc_t), \
   .ep_ac_in.bDescriptorType     = MIDI_CS_ENDPOINT, /* 0x25 */ \
   .ep_ac_in.bDescriptorSubtype  = MS_GENERAL, /* 0x01 */ \
   .ep_ac_in.embJacks            = 6, \
   .ep_ac_in.jackID              = { 21, 22, 23, 24, 25, 26 }, \
   }

#define UDI_MIDI_DESC7 { \
   .iface                        = UDI_MIDI_IFACE_DESC, \
   .msifc.bLength                = sizeof(usb_midi_stream_desc_t), \
   .msifc.bDescriptorType        = MIDI_CS_INTERFACE, /* 0x24 */ \
   .msifc.bDescriptorSubtype     = AC_HEADER, /* 0x01 */ \
   .msifc.bcdADc                 = LE16(0x0100), \
   .msifc.wTotalLength           = LE16(sizeof(udi_midi_desc7_t) - sizeof(usb_iface_desc_t)), \
   .jin_int                      = { \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 1), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 2), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 3), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 4), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 5), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 6), \
        USB_MIDI_IN_JACK(JACK_TYPE_EMBEDDED, 7), \
   }, \
   .jin_ext                      = { \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 11), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 12), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 13), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 14), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 15), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 16), \
        USB_MIDI_IN_JACK(JACK_TYPE_EXTERNAL, 17), \
   }, \
   .jout_int                     = { \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 21, 11), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 22, 12), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 23, 13), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 24, 14), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 25, 15), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 26, 16), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EMBEDDED, 27, 17), \
   }, \
   .jout_ext                     = { \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 31, 1), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 32, 2), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 33, 3), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 34, 4), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 35, 5), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 36, 6), \
        USB_MIDI_OUT_JACK(JACK_TYPE_EXTERNAL, 37, 7), \
   }, \
   .ep_out.ep.bLength            = sizeof(usb_midi_jack_ep_desc_t),\
   .ep_out.ep.bDescriptorType    = USB_DT_ENDPOINT,\
   .ep_out.ep.bmAttributes       = USB_EP_TYPE_BULK,\
   .ep_out.ep.wMaxPacketSize     = LE16(64),\
   .ep_out.ep.bInterval          = 0,\
   .ep_out.ep.bEndpointAddress   = UDI_MIDI_EP_OUT,\
   .ep_out.refresh               = 0,\
   .ep_out.sync                  = 0,\
   .ep_ac_out.bLength            = sizeof(usb_midi_jack_ep_ac7_desc_t), \
   .ep_ac_out.bDescriptorType    = MIDI_CS_ENDPOINT, /* 0x25 */ \
   .ep_ac_out.bDescriptorSubtype = MS_GENERAL, /* 0x01 */ \
   .ep_ac_out.embJacks           = 7, \
   .ep_ac_out.jackID             = { 1, 2, 3, 4, 5, 6, 7 }, \
   .ep_in.ep.bLength             = sizeof(usb_midi_jack_ep_desc_t),\
   .ep_in.ep.bDescriptorType     = USB_DT_ENDPOINT,\
   .ep_in.ep.bmAttributes        = USB_EP_TYPE_BULK,\
   .ep_in.ep.wMaxPacketSize      = LE16(64),\
   .ep_in.ep.bInterval           = 0,\
   .ep_in.ep.bEndpointAddress    = UDI_MIDI_EP_IN,\
   .ep_in.refresh                = 0,\
   .ep_in.sync                   = 0,\
   .ep_ac_in.bLength             = sizeof(usb_midi_jack_ep_ac7_desc_t), \
   .ep_ac_in.bDescriptorType     = MIDI_CS_ENDPOINT, /* 0x25 */ \
   .ep_ac_in.bDescriptorSubtype  = MS_GENERAL, /* 0x01 */ \
   .ep_ac_in.embJacks            = 7, \
   .ep_ac_in.jackID              = { 21, 22, 23, 24, 25, 26, 27 }, \
   }

//! Global structure which contains standard UDI API for UDC
extern UDC_DESC_STORAGE udi_api_t udi_api_midi;
extern UDC_DESC_STORAGE udi_api_t udi_api_audio_ctrl;

