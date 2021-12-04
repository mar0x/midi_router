#pragma once

#include "blink_state.h"

struct pulse_state_t : public blink_state_t {
    enum {
        START = 0x2000,
        MAX = 0xFFFF - START
    };

    pulse_state_t() {
        state = START;
    }

    void next() {
        ++state;

        if (state >= MAX) { state = START; }
    }

    template<typename T>
    void write() {
        T::write(get());

        next();
    }
};
