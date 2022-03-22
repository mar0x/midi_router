#pragma once

extern "C" {
#include "asf.h"
}

#include "ring.h"

namespace midi {

extern volatile uint8_t rx_ready;

struct port_stat_t {
    enum {
        MAX_FIELD = 8,
    };

    uint32_t operator[](uint8_t i) const {
        return (&rcv_bytes)[i];
    }

    void reset() {
        rcv_bytes = 0;
        rcv_msgs = 0;
        snd_bytes = 0;
        snd_msgs = 0;
        snd_ovf = 0;
        stall_ms = 0;
        stall_bytes = 0;
        stall_msgs = 0;
    }

    uint32_t rcv_bytes = 0;
    uint32_t rcv_msgs = 0;
    uint32_t snd_bytes = 0;
    uint32_t snd_msgs = 0;
    uint32_t snd_ovf = 0;
    uint32_t stall_ms = 0;
    uint32_t stall_bytes = 0;
    uint32_t stall_msgs = 0;

    uint32_t stall_start = 0;
};

extern port_stat_t port_stat[MIDI_PORTS + 1];
extern bool port_stat_update;

extern bool port_mon;
extern bool port_in_mon[MIDI_PORTS];
extern bool port_out_mon[MIDI_PORTS];

void init();
void splitter();

uint8_t send(uint8_t port, const uint8_t *buf, uint8_t size);

void start_mon();
void stop_mon();

}

extern void midi_process_byte(uint8_t port, uint8_t data);
extern void midi_send_ready(uint8_t port, uint8_t size);

template<uint8_t ID>
struct rx_midi_traits {
    enum {
        id = ID,
    };

    static inline void on_rx(uint8_t d) {
        midi_process_byte(id, d);
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
