
#include "midi.h"
#include "cdc_print.h"
#include "timer.h"
#include "midi_cmd.h"
#include "midi_filter.h"
#include "midi_channelizer.h"
#include "midi_normalizer.h"
#include "midi_mixer.h"
#include "midi_collector.h"
#include "bit_count.h"
#include "usb.h"
#include "ui.h"

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

mixer_t mixer[MIDI_OUT_PORTS];

struct process_rx_t {
    static void process(uint8_t src, uint8_t data, uint8_t ds);
    static void drop(uint8_t src, uint8_t data, uint8_t ds);
};

normalizer_t<process_rx_t> in_port_normalizer[MIDI_IN_PORTS];

stateful_filter_t in_port_filter[MIDI_IN_PORTS];
channelizer_t in_port_channelizer[MIDI_IN_PORTS];

stateful_filter_t out_port_filter[MIDI_OUT_PORTS + USB_OUT_JACKS];
channelizer_t out_port_channelizer[MIDI_OUT_PORTS + USB_OUT_JACKS];

struct send_usb_t {
    static void process(uint8_t dst, const midi_cmd_t &cmd, uint8_t ds) {
        usb::send(dst, cmd, ds);
    }

    static void drop(uint8_t dst, const midi_cmd_t &cmd, uint8_t ds) {
    }
};

collector_t<send_usb_t> cmd_collector[MIDI_IN_PORTS];

using route_mask_t = typename bit_count<MIDI_ROUTES>::type;
using out_port_mask_t = typename bit_count<MIDI_OUT_PORTS + USB_OUT_JACKS>::type;

route_mask_t in_port_routing[MIDI_IN_PORTS];
out_port_mask_t in_port_out[MIDI_IN_PORTS];

filter_t route_filter[MIDI_ROUTES];

out_port_mask_t route_out[MIDI_ROUTES];
out_port_mask_t in_port_router_state[MIDI_IN_PORTS];

out_port_mask_t router(uint8_t src, uint8_t data, uint8_t ds) {
    if (ds == CMDID_NONE) {
        return in_port_router_state[src];
    }

    out_port_mask_t res = in_port_out[src];

    route_mask_t rmask = in_port_routing[src];
    for(uint8_t route = 0; rmask != 0 && route < MIDI_ROUTES;
        ++route, rmask = rmask >> 1) {
        if ((rmask & 0x01) == 0) continue;

        if (route_filter[route](data, ds)) {
            res |= route_out[route];
        }
    }

    if (ds < CMDID_SYS_RT) {
        in_port_router_state[src] = res;
    }

    return res;
}

void process_rx_t::process(uint8_t src, uint8_t data, uint8_t ds) {
    if (in_port_filter[src](data, ds)) {
        // TODO: blink & count
        in_port_channelizer[src](data, ds);

        out_port_mask_t mask = router(src, data, ds);
        out_port_mask_t out_mask = mask & ((1 << (MIDI_OUT_PORTS + 1)) - 1);

        for(uint8_t dst = 0; out_mask != 0 && dst < MIDI_OUT_PORTS;
            ++dst, out_mask = out_mask >> 1) {
            if ((out_mask & 0x01) == 0) continue;

            uint8_t out_data = data;

            out_port_channelizer[dst](out_data, ds);
            if (out_port_filter[dst](out_data, ds)) {
                if (mixer[dst](src, out_data, ds)) {
                    ui::tx_data(out_data == CMD_SYS_ACTIVE_S, dst);
                    // TODO: blink & count
                } else {
                    // TODO: count
                }
            } else {
                // TODO: count
            }
        }

        out_mask = (mask >> MIDI_OUT_PORTS) & ((1 << (USB_OUT_JACKS + 1)) - 1);

        for(uint8_t dst = MIDI_OUT_PORTS; out_mask != 0 && dst < MIDI_OUT_PORTS + USB_OUT_JACKS;
            ++dst, out_mask = out_mask >> 1) {
            if ((out_mask & 0x01) == 0) continue;

            uint8_t out_data = data;

            out_port_channelizer[dst](out_data, ds);
            if (out_port_filter[dst](out_data, ds)) {
                cmd_collector[src](dst, out_data, ds);
            } else {
                // TODO: count
            }
        }
    } else {
        // TODO: count
    }
}

void process_rx_t::drop(uint8_t src, uint8_t data, uint8_t ds) {
    // TODO: blink & count
}

void process_rxc(uint8_t src, uint8_t data, bool ferr) {
    ui::rx_data(data == CMD_SYS_ACTIVE_S, src);
    // TODO: count

    mon(src, true, &data, 1);

    uint8_t ds = midi_cmd_serial(data);

    in_port_normalizer[src](src, data, ds);
}

void update_timer(unsigned long t) {
    crit_sec cs;

    uint8_t port;

    // TODO: blink
    // TODO: stat

    for (uint8_t i = 0; i < MIDI_OUT_PORTS; ++i) {
        mixer[i].update_timer(t, port);
    }
}

void dump_state() {
    crit_sec cs;
//    merger_state.dump();
}

}
