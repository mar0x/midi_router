
#pragma once

#include <stdint.h>

struct usb_midi_event_t {
    uint8_t header;
    union {
        struct {
            uint8_t byte1;
            uint8_t byte2;
            uint8_t byte3;
        };
        uint8_t data[3];
    };

    void reset() { header = 0; }
    bool empty() const { return header == 0; }
    uint8_t jack() const { return (header & 0xF0) >> 4; }
    uint8_t size() const {
        static const uint8_t cin2size[16] =
            { 0, 0, 2, 3, 3, 1, 2, 3, 3, 3, 3, 3, 2, 2, 3, 1 };

        return cin2size[header & 0x0F];
    }

    operator bool() const { return header != 0; }
};

