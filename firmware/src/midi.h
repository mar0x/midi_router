#pragma once

extern "C" {
#include "asf.h"
}

namespace midi {

extern volatile uint8_t rx_ready;

extern volatile bool port_ready[MIDI_PORTS];
extern uint32_t port_in_bytes[MIDI_PORTS];
extern uint32_t port_out_bytes[MIDI_PORTS];
extern bool port_in_mon[MIDI_PORTS];
extern bool port_out_mon[MIDI_PORTS];

void init();
void splitter();

bool port_read(uint8_t port, uint8_t &data);
bool recv(uint8_t &port, uint8_t &data);
uint8_t send(uint8_t port, const uint8_t *buf, uint8_t size);

}
