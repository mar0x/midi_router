
#pragma once

#include <stdint.h>
#include "midi_cmd.h"

namespace midi {

template<typename LISTENER>
struct normalizer_t {
    using listener_t = LISTENER;

    void reset() {
        last_cmd = 0;
        pending_bytes = 0;
    }

    void operator()(uint8_t src, uint8_t data, uint8_t ds) {
        if (ds == CMDID_NONE) {
            uint8_t p = pending_bytes;
            if (p != 0) {
                if (p != 0xFF) {
                    pending_bytes = --p;
                }
            } else {
                if (last_cmd != 0) {
                    pending_bytes = midi_cmd_t::cmdid2size(last_ds) - 2;

                    listener_t::process(src, last_cmd, last_ds);
                } else {
                    listener_t::drop(src, data, ds);
                    return;
                }
            }
        } else {
            if (ds < CMDID_SYS_RT) {
                uint8_t s = midi_cmd_t::cmdid2size(ds);

                if (s != 0 && s != 0xFF) {
                    --s;
                }

                last_cmd = (ds < CMDID_SYS && s != 0) ? data : 0;
                last_ds = ds;

                pending_bytes = s;
            }
        }

        listener_t::process(src, data, ds);
    }

private:
    uint8_t last_cmd;
    uint8_t last_ds;
    uint8_t pending_bytes;
};

}
