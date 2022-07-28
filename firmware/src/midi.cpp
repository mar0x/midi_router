
#include "midi.h"

namespace midi {

const char *port_stat_t::title[port_stat_t::MAX_FIELD] = {
    "Rbyte", "Rmsg", "Rovf", "Rerr", "Rcmd", "Rto",
    "Sbyte", "Smsg", "Sovf",
    "STms", "STbyte", "STmsg",
};

const uint8_t port_stat_t::title_len[port_stat_t::MAX_FIELD] = {
    5, 4, 4, 4, 4, 3,
    5, 4, 4,
    4, 6, 5,
};

port_stat_t port_stat[MIDI_PORTS + 1];
bool port_stat_update = false;

bool port_in_mon[MIDI_PORTS];
bool port_out_mon[MIDI_PORTS];

bool port_mon = false;

artl::timer<> pending_timer;

process_byte_t on_rx_complete;
process_dre_t on_dre;

void start_mon() {
    for (uint8_t i = 0; i < MIDI_PORTS; ++i) {
        port_in_mon[i] = true;
        port_out_mon[i] = true;
    }
    port_mon = true;
}

void stop_mon() {
    for (uint8_t i = 0; i < MIDI_PORTS; ++i) {
        port_in_mon[i] = false;
        port_out_mon[i] = false;
    }
    port_mon = false;
}

void dummy_process_dre(uint8_t port) { }

}
