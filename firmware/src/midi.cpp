
#include "midi.h"

namespace midi {

volatile uint8_t rx_ready;

port_stat_t port_stat[MIDI_PORTS + 1];
bool port_stat_update = false;

bool port_in_mon[MIDI_PORTS];
bool port_out_mon[MIDI_PORTS];

bool port_mon = false;

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

}
