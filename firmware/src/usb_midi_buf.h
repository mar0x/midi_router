
#pragma once

#include "usb_midi_event.h"

struct usb_midi_buf_t {
    bool empty() const { return start == end; }
    bool full() const { return end >= capacity() / sizeof(usb_midi_event_t); }

    const usb_midi_event_t& first() const {
        return ev[start];
    }

    usb_midi_event_t& push() {
        return ev[end++];
    }

    void pop() { ++start; }
    void received(uint8_t s) {
        start = 0;
        end = s / sizeof(usb_midi_event_t);
        stall = false;
    }
    void set_stall() { stall = true; }

    void reset() {
        start = 0;
        end = 0;
        stall = false;
    }

    operator uint8_t*() { return raw; }
    uint8_t size() const { return (end - start) * sizeof(usb_midi_event_t); }
    static constexpr uint8_t capacity() { return sizeof(raw); }

    union {
        uint8_t raw[64];
        usb_midi_event_t ev[ sizeof(raw) / sizeof(usb_midi_event_t) ];
    };

    uint8_t start = 0;
    uint8_t end = 0;
    bool stall = false;
};
