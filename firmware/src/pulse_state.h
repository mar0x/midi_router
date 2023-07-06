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

    void start() { state = START; }

    void next() {
        state += STEP / 4;

        if (state >= MAX) { state = START; }
    }

    template<typename T>
    void write() {
        bool v = get();
        if (v != last_write) {
            last_write = v;
            T::write(v);
        }

        next();
    }
};
