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

struct merger_state_t {
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

    bool port_queue_empty() const { return port_queue_mask == 0; }
    uint8_t port_queue_front() const { return port_queue.front(); }

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
        iram_size_t csize = udi_cdc_get_free_tx_buffer();

        if (csize >= (iram_size_t) (4 + 2 + 4 + 2 + 11 + 2 + 2 + 2 + 2)) {
            _cdc_print("dre:"); _cdc_print_hex(dre_wait_mask);
            _cdc_print(",pb:", pending_bytes, ",pq(m/s/f):");
            _cdc_print_hex(port_queue_mask);
            _cdc_println("/", port_queue.size(), "/", port_queue.front());
        }
    }

    uint8_t dre_wait_mask = 0;

    ring<MIDI_PORTS + 2, uint8_t> port_queue;
    uint8_t port_queue_mask = 0;

    uint8_t pending_bytes = 0;
    uint8_t pending_port = 0;
    unsigned long last_byte_time = 0;
    bool pending_timeout_fired = false;

    ring<16, uint8_t> port_byte_queue[MIDI_PORTS + 1];
    ring<8, uint8_t> rt_queue;

    unsigned long last_dump_time = 0;
};

template<typename SRC, typename DST>
struct merger_t {
    enum {
        PENDING_TIMEOUT_MS = 4,
    };

    static inline void enable() {
        state.dre_wait_mask = 0;

        state.port_queue_clear();
        state.pending_bytes = 0;

        state.last_byte_time = 0;
        state.pending_timeout_fired = false;
        pending_timer.cancel();

        for (uint8_t i = 0; i < MIDI_PORTS + 1; ++i) {
            state.port_byte_queue[i].clear();
        }
        state.rt_queue.clear();

        state.last_dump_time = 0;

        port_stat_update = true;
    }

    static inline void disable() {
        state.dre_wait_mask = 0;

        port_stat_update = true;
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

        if (q.empty() && !is_cmd && (state.port_queue_mask == 0 || state.port_queue_front() != port)) {
            uint8_t c2 = q.data[(q.begin + q.capacity - 2) % q.capacity];
            uint8_t c3 = q.data[(q.begin + q.capacity - 3) % q.capacity];
            uint8_t c4 = q.data[(q.begin + q.capacity - 4) % q.capacity];

            q.push_back(data);
            q.pop_front();

            if ((midi_cmd_t::command(c3) == CMD_NOTE_ON && data == c2) ||
                (midi_cmd_t::command(c4) == CMD_NOTE_ON && data == 0)) {
                ++pstat.stall_bytes;
            } else {
                ui::rx_error(port);
                ++pstat.rcv_cmd;

                dump_queues(millis_(), port);
            }

            return;
        }

        state.port_queue_add(port);
        q.push_back(data);

        if (state.dre_wait_mask == 0 && state.port_queue_front() == port) {
            process_queue();

            return;
        }

        if (state.last_byte_time != 0 && !pending_timer.active()) {
            state.pending_port = state.port_queue_front();
            pending_timer.schedule(state.last_byte_time + PENDING_TIMEOUT_MS);
        }
    }

    static void process_dre(uint8_t port) {
        state.dre_wait_mask &= ~(1 << port);

        if (state.dre_wait_mask == 0) {
            if (state.pending_timeout_fired) {
                state.pending_timeout_fired = false;

                if (!state.port_queue_empty() && state.pending_port == state.port_queue_front()) {
                    state.port_queue_pop();
                }
            }

            process_queue();
        }
    }

    static void pending_timeout() {
        state.last_byte_time = 0;

        if (state.port_queue_empty() || state.pending_port != state.port_queue_front()) {
            return;
        }

        ui::rx_error(state.pending_port);
        ++port_stat[state.pending_port].rcv_to;

        dump_queues(millis_(), state.pending_port);

        if (state.dre_wait_mask != 0) {
            state.pending_timeout_fired = true;
            return;
        }

        state.port_queue_pop();

        process_queue();
    }

    static void dump() {
        if (cdc_dtr) {
            state.dump();
        }
    }

private:
    static void send(uint8_t data) {
        DST::write_byte(data);

        state.dre_wait_mask = DST::tx_ring_non_empty_mask();

        if (is_midi_cmd(data)) {
            ui::tx_data_mask(data == CMD_SYS_ACTIVE_S, DST::tx_mask);
        }
    }

    static void process_queue() {
        do {
            uint8_t data;

            if (!pop_queue(data)) {
                return;
            }

            send(data);
        } while(state.dre_wait_mask == 0);
    }

    static bool pop_queue(uint8_t &data) {
        if (!state.rt_queue.empty()) {
            data = state.rt_queue.pop_front();
            return true;
        }

        if (state.port_queue_empty()) {
            return false;
        }

        uint8_t port = state.port_queue_front();
        auto &q = state.port_byte_queue[port];

        if (q.empty()) {
            return false;
        }

        data = q.pop_front();

        if (update_pending_bytes(data) == 0) {
            state.port_queue_pop();
        }

        return true;
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
                state.pending_port = state.port_queue_front();
                pending_timer.schedule(state.last_byte_time + PENDING_TIMEOUT_MS);
            }
        } else {
            if (state.last_byte_time != 0) {
                state.last_byte_time = 0;
                pending_timer.cancel();
            }
        }

        return state.pending_bytes;
    }

    static inline void dump_queues(unsigned long t, uint8_t port) {
        if (!cdc_dtr) return;

        if (t - state.last_dump_time < 1000) {
            return;
        }
        state.last_dump_time = t;

        iram_size_t csize = udi_cdc_get_free_tx_buffer();

        static const iram_size_t port_queue_dump_size = 2 + 2 * state.port_queue.capacity + 2;

        if (csize >= port_queue_dump_size) {
            _cdc_print(state.port_queue.size(), ":");
            for(uint8_t i = 0; i < state.port_queue.capacity; ++i) {
                _cdc_print(state.port_queue.data[i],
                    i == state.port_queue.begin ? "<" : " ");
            }
            _cdc_print_eol();
            csize -= port_queue_dump_size;
        }

        static const iram_size_t port_byte_queue_dump_size = 2 + 3 + 3 * state.port_byte_queue[0].capacity + 2;

        for (uint8_t p = 0; p < MIDI_PORTS; ++p) {
            if (csize < port_byte_queue_dump_size) break;

            auto &q = state.port_byte_queue[p];

            _cdc_print(p, p == port ? "*" : " ");
            _cdc_print_w(q.size(), 2);
            _cdc_print(":");

            for (uint8_t i = 0; i < q.capacity; ++i) {
                _cdc_print_hex(q.data[i]);
                _cdc_print(i == q.begin ? "<" : " ");
            }
            _cdc_print_eol();

            csize -= port_byte_queue_dump_size;
        }

        state.dump();
    }

    static merger_state_t state;
};

}
