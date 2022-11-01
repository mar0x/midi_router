#pragma once

extern "C" {
#include "asf.h"
}

#include "midi_mixer.h"
#include "midi_filter.h"
#include "midi_channelizer.h"

enum {
    MIDI_ROUTES = 16,
};

namespace midi {

struct port_stat_t {
    enum {
        MAX_FIELD = 12,
    };

    static const char *title[MAX_FIELD];
    static const uint8_t title_len[MAX_FIELD];

/*
    port_stat_t() {
        rcv_bytes = 0;
        //reset();
    }
*/

    uint32_t operator[](uint8_t i) const {
        return (&rcv_bytes)[i];
    }

    void reset() {
        memset(&rcv_bytes, 0, sizeof(rcv_bytes) * MAX_FIELD);
/*
        for (uint8_t i = 0; i < MAX_FIELD; ++i) {
            (&rcv_bytes)[i] = 0;
        }
*/
    }

    uint32_t rcv_bytes;
    uint32_t rcv_msgs;
    uint32_t rcv_ovf;
    uint32_t rcv_err;
    uint32_t rcv_cmd;
    uint32_t rcv_to;

    uint32_t snd_bytes;
    uint32_t snd_msgs;
    uint32_t snd_ovf;
    uint32_t stall_ms;
    uint32_t stall_bytes;
    uint32_t stall_msgs;

    uint32_t stall_start;
};

extern port_stat_t port_stat[MIDI_PORTS + 1];
extern bool port_stat_update;

extern bool mon_enabled;
extern bool port_mon[2][MIDI_PORTS];

extern mixer_t mixer[MIDI_OUT_PORTS];

extern stateful_filter_t in_port_filter[MIDI_IN_PORTS];
extern channelizer_t in_port_channelizer[MIDI_IN_PORTS];

extern stateful_filter_t out_port_filter[MIDI_OUT_PORTS + USB_OUT_JACKS];
extern channelizer_t out_port_channelizer[MIDI_OUT_PORTS + USB_OUT_JACKS];

extern filter_t route_filter[MIDI_ROUTES];

void setup();

void process_rxc(uint8_t src, uint8_t data, bool ferr);
void update_timer(unsigned long t);

void dump_state();

void start_mon();
void stop_mon();
void mon(uint8_t port, bool dir_in, const uint8_t *b, uint8_t size);

template<typename T>
void setup_mixer() {
    mixer[T::tx_traits::id].setup(T::tx_traits::id, T::write_byte_get_dre);
}

}

extern void midi_process_byte(uint8_t port, uint8_t data, bool ferr);
extern void midi_send_ready(uint8_t port, uint8_t size);

template<uint8_t ID>
struct rx_midi_traits {
    enum {
        id = ID,
    };
};

template<uint8_t ID>
struct tx_midi_traits {
    enum {
        id = ID,
    };
};
