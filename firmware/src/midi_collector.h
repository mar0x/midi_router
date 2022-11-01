
#pragma once

#include <stdint.h>
#include "midi_cmd.h"

namespace midi {

template<typename LISTENER>
struct collector_t {
    using listener_t = LISTENER;

    void reset() {
        cmd.reset();
        cmd_ds = CMDID_NONE;
    }

    void operator()(uint8_t port, uint8_t data, uint8_t ds) {
        if (ds >= CMDID_SYS_RT) {
            listener_t::process(port, data, ds);
            return;
        }

        if (ds == CMDID_SYS_EX_END && cmd_ds == CMDID_SYS_EX) {
            cmd.append(data);
            goto process_and_reset;
        }

        if (ds != CMDID_NONE && cmd_ds == CMDID_SYS_EX) {
            goto process_and_reset;
        }

        if (ds == CMDID_NONE) {
            if (cmd_ds != CMDID_NONE) {
                cmd.append(data);
                if (cmd_ds == CMDID_SYS_EX && cmd.full()) {
                    listener_t::process(port, cmd, cmd_ds);
                    cmd.reset();
                    return;
                }
            } else {
                listener_t::drop(port, data, ds);
                return;
            }
        } else {
            if (cmd_ds != CMDID_NONE) {
                listener_t::drop(port, cmd, cmd_ds);
                reset();
            }

            cmd.append(data);
            cmd_ds = ds;
        }

        if (cmd.size() < cmd.cmdid2size(cmd_ds)) {
            return;
        }

process_and_reset:
        listener_t::process(port, cmd, cmd_ds);
        reset();
    }

    midi_cmd_t cmd;
    uint8_t cmd_ds;
};

}
