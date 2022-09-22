#pragma once

extern "C" {
#include "asf.h"
}

#include "midi.h"
#include "midi_cmd.h"
#include "ui.h"

namespace midi {

template<typename SRC, typename DST>
struct bit_splitter_t {
    static inline void enable() {
        DST::disable_tx();
        DST::enable_pin_ch();
    }

    static inline void disable() {
        DST::disable_pin_ch();
        DST::enable_tx();
    }

    static void process_bit(uint8_t port, bool b) {
        if (b) {
            DST::tx_high();
        } else {
            DST::tx_low();
        }
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
    }
};

}
