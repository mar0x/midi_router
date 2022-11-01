
#pragma once

#include <stdint.h>
#include "midi_cmd.h"

namespace midi {

struct normalizer_t {
    void reset() {
        last_cmd = 0;
        pending_bytes = 0;
    }

    uint8_t operator()(uint8_t data, uint8_t &ds) {
        if (ds > CMDID_NONE) {
            if (ds >= CMDID_SYS_RT) {
                return 0;
            }

            if (ds < CMDID_SYS) {
                last_cmd = data;
            } else {
                last_cmd = 0;
            }

            uint8_t s = midi_cmd_t::cmdid2size(ds);

            if (s != 0 && s != 0xFF) { --s; }

            pending_bytes = s;

            return 0;
        }

        uint8_t p = pending_bytes;
        if (p != 0) {
            if (p != 0xFF) {
                pending_bytes = --p;
            }

            return 0;
        }

        uint8_t c = last_cmd;
        if (c) {
            ds = midi_cmd_serial(c);
            uint8_t s = midi_cmd_t::cmdid2size(ds);

            if (s != 0 && s != 0xFF) { --s; }
            if (s != 0 && s != 0xFF) { --s; }

            pending_bytes = s;

            return c;
        }

        return 1;
    }

private:
    uint8_t last_cmd;
    uint8_t pending_bytes;
};

}
