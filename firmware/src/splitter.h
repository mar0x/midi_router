#pragma once

extern "C" {
#include "asf.h"
}

#include "ring.h"
#include "midi.h"
#include "midi_cmd.h"
#include "ui.h"
#include "timer.h"
#include "cdc_print.h"

namespace midi {

struct splitter_state_t {
    void port_queue_clear() {
        port_queue_mask = 0;
        port_queue.clear();
    }

    void port_queue_add(uint8_t port) {
        if ((port_queue_mask & (1 << port)) == 0) {
            port_queue_mask |= (1 << port);

            port_queue.push_back(port);
        }
    }

    uint8_t port_queue_pop() {
        uint8_t port = port_queue.pop_front();
        if (port_byte_queue[port].empty()) {
            port_queue_mask &= ~(1 << port);
        } else {
            port_queue.push_back(port);
        }
        return port;
    }

    void dump() {
        cdc_println("drew: ", dre_wait_mask);
        cdc_println("bp:  ", pending_bytes);
        cdc_println("pq(m/s/f): ", port_queue_mask, " ", port_queue.size(), " ", port_queue.front());
    }

    uint8_t dre_wait_mask = 0;

    ring<MIDI_PORTS + 2, uint8_t> port_queue;
    uint8_t port_queue_mask = 0;

    uint8_t pending_bytes = 0;
    unsigned long last_byte_time = 0;

    ring<32, uint8_t> port_byte_queue[MIDI_PORTS + 1];
    ring<8, uint8_t> rt_queue;
};

template<typename UL>
struct splitter_t {
    enum {
        ALL_PORTS_MASK = ((1 << MIDI_PORTS) - 1),
        PENDING_TIMEOUT_MS = 2,
    };

    static void setup() {
        UL::setup();
        UL::rxc_int_hi();
    }

    static inline void enable() {
        setup();

        state.dre_wait_mask = 0;

        state.port_queue_clear();
        state.pending_bytes = 0;

        state.last_byte_time = 0;
        pending_timer.cancel();

        for (uint8_t i = 0; i < MIDI_PORTS + 1; ++i) {
            state.port_byte_queue[i].clear();
        }
        state.rt_queue.clear();

        port_stat_update = true;
    }

    static inline void disable() {
        state.dre_wait_mask = 0;

        setup();

        port_stat_update = true;
    }

    static void process_bit(uint8_t port, bool b) {
        if (b) {
            UL::tx_high();
        } else {
            UL::tx_low();
        }
    }

    static void rx_complete(uint8_t port, uint8_t data, bool ferr) {
        port_stat_t &pstat = port_stat[port];

        ++pstat.rcv_bytes;
        port_stat_update = true;

        if (ferr) {
            ui::rx_error(port);
            ++pstat.rcv_err;
        } else {
            ui::rx_data(data == CMD_SYS_ACTIVE_S, port);
        }

        bool is_cmd = is_midi_cmd(data);

        if (is_cmd) {
            ++pstat.rcv_msgs;
        }

        if (is_midi_rt(data)) {
            if (state.rt_queue.full()) {
                state.rt_queue.pop_front();
                ui::rx_error(port);
                ++pstat.rcv_ovf;
            }

            state.rt_queue.push_back(data);

            if (state.dre_wait_mask == 0) {
                process_queue();
            }

            return;
        }

        auto &q = state.port_byte_queue[port];

        if (q.full()) {
            ui::rx_error(port);
            ++pstat.rcv_ovf;

            return;
        }

        if (q.empty() && !is_cmd && (state.port_queue_mask == 0 || state.port_queue.front() != port)) {
            ui::rx_error(port);
            ++pstat.rcv_cmd;

            return;
        }

        state.port_queue_add(port);
        q.push_back(data);

        if (state.dre_wait_mask == 0 && state.port_queue.front() == port) {
            process_queue();

            return;
        }

        if (state.last_byte_time != 0 && !pending_timer.active()) {
            pending_timer.schedule(state.last_byte_time + PENDING_TIMEOUT_MS);
        }
    }

    static void process_dre(uint8_t port) {
        state.dre_wait_mask &= ~(1 << port);

        if (state.dre_wait_mask == 0) {
            process_queue();
        }
    }

    static void pending_timeout() {
        if (state.dre_wait_mask != 0) {
            return;
        }

        uint8_t port = state.port_queue_pop();

        ui::rx_error(port);
        ++port_stat[port].rcv_to;

        process_queue();
    }

    static void dump() {
        state.dump();
    }

private:
    static void send(uint8_t data) {
        UL::write_byte(data);

        if (!UL::tx_ring_empty()) {
            state.dre_wait_mask = ALL_PORTS_MASK;
        }

        if (is_midi_cmd(data)) {
            if (data == CMD_SYS_ACTIVE_S) {
                ui::tx_active();
            } else {
                ui::tx_blink();
            }
        }
    }

    static void process_queue() {
        uint8_t data;

        if (!state.rt_queue.empty()) {
            data = state.rt_queue.pop_front();
        } else {
            if (state.port_queue.empty()) {
                return false;
            }

            uint8_t port = state.port_queue.front();
            auto &q = state.port_byte_queue[port];

            if (q.empty()) {
                return false;
            }

            data = q.pop_front();

            if (update_pending_bytes(data) == 0) {
                state.port_queue_pop();
            }
        }

        send(data);
    }

    static inline uint8_t update_pending_bytes(uint8_t data) {
        uint8_t s = midi_cmd_t::cmd_size(data);
        if (s == 0) {
            if (state.pending_bytes != 0 && state.pending_bytes != 0xFF) {
                --state.pending_bytes;
            }
        } else {
            if (data != CMD_SYS_EX) {
                state.pending_bytes = s - 1;
            } else {
                state.pending_bytes = 0xFF;
            }
        }

        if (state.pending_bytes != 0) {
            state.last_byte_time = millis_();

            if (state.port_queue.size() > 1) {
                pending_timer.schedule(state.last_byte_time + PENDING_TIMEOUT_MS);
            }
        } else {
            state.last_byte_time = 0;
            pending_timer.cancel();
        }

        return state.pending_bytes;
    }

    static splitter_state_t state;
};

}
