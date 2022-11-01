
#pragma once

#include "midi_cmd.h"

namespace usb {

bool send(uint8_t jack, const midi_cmd_t& c, uint8_t ds);
bool process_rx();
void enable();
void disable();

}
