
#pragma once

#define HAVE_COMMAND_FILTER 0
#define HAVE_CHANNEL_FILTER 0

#include <stdint.h>
#include "midi_cmd.h"

namespace midi {

struct command_filter_t {
#if HAVE_COMMAND_FILTER
    void reset() {
        command_filter = CMDMASK_ALL;
    }

    bool operator()(uint8_t data, uint8_t ds) const {
        return (command_filter & (1 << ds)) != 0;
    }
#else
    void reset() { }
    bool operator()(uint8_t data, uint8_t ds) const { return true; }
#endif

    uint32_t command_filter;
};

struct channel_filter_t {
#if HAVE_CHANNEL_FILTER
    void reset() {
        channel_filter = 0xFFFFU;
    }

    bool operator()(uint8_t data, uint8_t ds) const {
        if (ds < CMDID_SYS) {
            uint8_t ch = data & 0x0F;
            return (channel_filter & (1 << ch)) != 0;
        }
        return true;
    }
#else
    void reset() { }
    bool operator()(uint8_t data, uint8_t ds) const { return true; }
#endif

    uint16_t channel_filter;
};

struct filter_t : public command_filter_t, channel_filter_t {
    void reset() {
        command_filter_t::reset();
        channel_filter_t::reset();
    }

    bool operator()(uint8_t data, uint8_t ds) const {
        return command_filter_t::operator()(data, ds) &&
            channel_filter_t::operator()(data, ds);
    }
};

struct stateful_filter_t : public filter_t {
#if (HAVE_COMMAND_FILTER || HAVE_CHANNEL_FILTER)
    bool operator()(uint8_t data, uint8_t ds) {
        if (ds == CMDID_NONE) { return state; }

        bool res = filter_t::operator()(data, ds);

        if (ds < CMDID_SYS_RT) { state = res; }

        return res;
    }

    bool state;
#else
    bool operator()(uint8_t data, uint8_t ds) const { return true; }
#endif
};

}
