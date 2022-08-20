#pragma once

extern "C" {
#include "asf.h"
}

#include "ring.h"
#include "artl/timer.h"

namespace midi {

struct port_stat_t {
    enum {
        MAX_FIELD = 12,
    };

    static const char *title[MAX_FIELD];
    static const uint8_t title_len[MAX_FIELD];

    uint32_t operator[](uint8_t i) const {
        return (&rcv_bytes)[i];
    }

    void reset() {
        for (uint8_t i = 0; i < MAX_FIELD; ++i) {
            (&rcv_bytes)[i] = 0;
        }
    }

    uint32_t rcv_bytes = 0;
    uint32_t rcv_msgs = 0;
    uint32_t rcv_ovf = 0;
    uint32_t rcv_err = 0;
    uint32_t rcv_cmd = 0;
    uint32_t rcv_to = 0;
    uint32_t snd_bytes = 0;
    uint32_t snd_msgs = 0;
    uint32_t snd_ovf = 0;
    uint32_t stall_ms = 0;
    uint32_t stall_bytes = 0;
    uint32_t stall_msgs = 0;

    uint32_t stall_start = 0;
};

using process_byte_t = void (*)(uint8_t port, uint8_t data, bool ferr);
using process_dre_t = void (*)(uint8_t port);

extern port_stat_t port_stat[MIDI_PORTS + 1];
extern bool port_stat_update;

extern bool port_mon;
extern bool port_in_mon[MIDI_PORTS];
extern bool port_out_mon[MIDI_PORTS];

extern artl::timer<> pending_timer;

extern process_byte_t on_rx_complete;
extern process_dre_t on_dre;

void init(process_byte_t cb = NULL);

uint8_t send(uint8_t port, const uint8_t *buf, uint8_t size);
void pending_timeout();

void dump_state();

void start_mon();
void stop_mon();

void dummy_process_dre(uint8_t port);

}

extern void midi_send_ready(uint8_t port, uint8_t size);

template<uint8_t ID>
struct rx_midi_traits {
    enum {
        id = ID,
    };

    static inline void on_rx(uint8_t d) {
        midi_process_byte(id, d, false);
    }
};

template<uint8_t ID>
struct tx_midi_traits {
    enum {
        id = ID,
    };

    static inline void tx_ready(uint8_t s) {
        midi_send_ready(id, s);
    }
};
