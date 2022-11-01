
#pragma once

#include <stdint.h>
#include "midi_cmd.h"

#define HAVE_CHANNELIZER 0

namespace midi {

struct channelizer_t {
#if HAVE_CHANNELIZER
    void reset() {
        channel_map[0] = 0x01;
        channel_map[1] = 0x23;
        channel_map[2] = 0x45;
        channel_map[3] = 0x67;
        channel_map[4] = 0x89;
        channel_map[5] = 0xAB;
        channel_map[6] = 0xCD;
        channel_map[7] = 0xEF;
    }

    void operator()(uint8_t &data, uint8_t ds) const {
        if (ds == CMDID_NONE || ds >= CMDID_SYS) {
            return;
        }

        uint8_t ch = data & 0x0F;
        uint8_t cm = channel_map[ch >> 1];

        if ((ch & 0x01) == 0) {
            cm = cm >> 4;
        } else {
            cm = cm & 0x0F;
        }

        data = (data & 0xF0) | cm;
    }
#else
    void reset() { }
    void operator()(uint8_t &data, uint8_t ds) const { }
#endif

    uint8_t channel_map[8];
};

}
