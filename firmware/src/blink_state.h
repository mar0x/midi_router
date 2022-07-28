#pragma once

#include <stdint.h>

struct blink_state_t {
    enum {
        START = 0x8000,
        MIN = 0x1000,
        ACTIVE = 0x1000,
        MIN_ACTIVE = ACTIVE - 0x100,
        STEP = 0x40,

        START_ERROR = 0x8000,
        MIN_ERROR = STEP,
    };

    static bool get(uint16_t state) {
        uint8_t p = state >> 8;

        if (p > 254) { p = 254; }
        if (p > 127) { p = 254 - p; }

        p += p;

        return ((state & 0xFF) <= p);
    }

    bool get() const {
        return error_state ? (state > (START_ERROR / 2)) : get(state);
    }

    inline __attribute__((always_inline))
    void start() { if (state <= MIN && !error_state) { state = START; } }

    void stop() { state = 0; }
    void active() {
        if (!error_state) {
            active_state = 0x80;

            if (state < MIN_ACTIVE) { state = ACTIVE; }
        }
    }

    void error() {
        if (!error_state) {
            state = START_ERROR,
            active_state = 0;
        }
        error_state = 5;
    }

    operator bool() const { return state != 0; }

    void next() {
        state -= STEP;

        if (active_state && state < MIN_ACTIVE) {
            state += 0x100;
            active_state -= 1;
        }

        if (error_state) {
            if (state) { state -= STEP; }
            if (state < MIN_ERROR) {
                state = START_ERROR;
                error_state -= 1;
            }
        }
    }

    template<typename T>
    void write() {
        if (state) {
            bool v = get();
            if (v != last_write) {
                last_write = v;
                T::write(v);
            }

            next();
        }
    }

    template<typename T>
    void force_write() {
        bool v = state ? get() : false;

        last_write = v;
        T::write(v);
    }

    uint16_t state = 0;
    uint8_t active_state = 0;
    uint8_t error_state = 0;
    bool last_write = false;
};
