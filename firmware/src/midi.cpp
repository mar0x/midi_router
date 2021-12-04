
#include "midi.h"
#include <crit_sec.h>

namespace midi {

volatile uint8_t rx_ready;

volatile bool port_ready[MIDI_PORTS];

uint32_t port_in_bytes[MIDI_PORTS];
uint32_t port_out_bytes[MIDI_PORTS];

bool port_in_mon[MIDI_PORTS];
bool port_out_mon[MIDI_PORTS];

bool recv(uint8_t &port, uint8_t &data) {
    uint8_t ready = rx_ready;

    for (uint8_t i = 0; i < MIDI_PORTS; ++i) {
        uint8_t n = (i + ready) % MIDI_PORTS;

        if (port_ready[n]) {
            crit_sec cs;

            if (port_read(n, data)) {
                ++port_in_bytes[n];
                port = n;
                return true;
            }

            port_ready[n] = false;
        }
    }
    return false;
}

}
