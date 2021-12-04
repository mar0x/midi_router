#pragma once

#include <stdint.h>

struct blink_state_t {
    enum {
        START = 0x8000,
        MIN = 0x2000,
        ACTIVE = 0x1800,
        MIN_ACTIVE = ACTIVE - 0x100,
        STEP = -4,
    };

    static bool get(uint16_t state) {
        uint8_t p = state >> 8;

        if (p > 254) { p = 254; }
        if (p > 127) { p = 254 - p; }

        p += p;

        return ((state & 0xFF) <= p);
    }

    bool get() const { return get(state); }

    inline __attribute__((always_inline))
    void start() { if (state <= MIN) { state = START; } }

    void stop() { state = 0; }
    void active() {
        active_state = 0x200;

        if (state < MIN_ACTIVE) { state = ACTIVE; }
    }

    operator bool() const { return state != 0; }

    void next() {
        state -= 4;

        if (active_state && state < MIN_ACTIVE) {
            state += 0x100;
            active_state -= 4;
        }
    }

    template<typename T>
    void write() {
        if (state) {
            T::write(get());

            next();
        }
    }

    uint16_t state = 0;
    uint16_t active_state = 0;
};
