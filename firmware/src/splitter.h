#pragma once

extern "C" {
#include "asf.h"
}

#include "midi.h"
#include "midi_cmd.h"
#include "ui.h"

namespace midi {

struct splitter_state_t {
    uint8_t dre_wait_mask = 0;

    ring<16, uint8_t> queue;
};

template<typename SRC, typename DST>
struct splitter_t {
    static inline void enable() {
        state.dre_wait_mask = 0;
        state.queue.clear();
    }

    static inline void disable() {
        state.dre_wait_mask = 0;
        state.queue.clear();
    }

    static void rx_complete(uint8_t port, uint8_t data, bool ferr) {
        port_stat_t &pstat = port_stat[port];

        ++pstat.rcv_bytes;
        port_stat_update = true;

        if (ferr) {
            ui::rx_error(port);
            ++pstat.rcv_err;
        } else {
            ui::rx_data(data == CMD_SYS_ACTIVE_S, port);
            ui::tx_data_mask(data == CMD_SYS_ACTIVE_S, DST::tx_mask);
        }

        if (is_midi_cmd(data)) {
            ++pstat.rcv_msgs;
        }

        if (state.queue.full()) {
            ui::rx_error(port);
            ++pstat.rcv_ovf;

            return;
        }

        state.queue.push_back(data);

        if (state.dre_wait_mask == 0) {
            process_queue();
        }
    }

    static void process_dre(uint8_t port) {
        state.dre_wait_mask &= ~(1 << port);

        if (state.dre_wait_mask == 0) {
            process_queue();
        }
    }

private:
    static void send(uint8_t data) {
        DST::write_byte(data);

        state.dre_wait_mask = DST::tx_ring_non_empty_mask();

        if (is_midi_cmd(data)) {
            ui::tx_data_mask(data == CMD_SYS_ACTIVE_S, DST::tx_mask);
        }
    }

    static void process_queue() {
        do {
            uint8_t data;

            if (!pop_queue(data)) {
                return;
            }

            send(data);
        } while(state.dre_wait_mask == 0);
    }

    static bool pop_queue(uint8_t &data) {
        if (state.queue.empty()) {
            return false;
        }

        data = state.queue.pop_front();

        return true;
    }

    static splitter_state_t state;
};

}
