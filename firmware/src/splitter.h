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

template<typename UL>
struct splitter_t {
    enum {
        ALL_PORTS_MASK = ((1 << MIDI_PORTS) - 1),
        PENDING_TIMEOUT_MS = 2,
    };

    void setup() {
        UL::setup();
        UL::rxc_int_hi();
    }

    inline void enable() {
        setup();

        dre_wait_mask = 0;

        port_queue_clear();
        pending_bytes = 0;

        last_byte_time = 0;
        pending_timer.cancel();

        for (uint8_t i = 0; i < MIDI_PORTS + 1; ++i) {
            port_byte_queue[i].clear();
        }
        rt_queue.clear();

        port_stat_update = true;
    }

    inline void disable() {
        dre_wait_mask = 0;

        setup();

        port_stat_update = true;
    }

    void process_bit(uint8_t port, bool b) {
        if (b) {
            UL::tx_high();
        } else {
            UL::tx_low();
        }
    }

    void rx_complete(uint8_t port, uint8_t data, bool ferr) {
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
            if (rt_queue.full()) {
                rt_queue.pop_front();
                ui::rx_error(port);
                ++pstat.rcv_ovf;
            }

            rt_queue.push_back(data);

            if (dre_wait_mask == 0) {
                process_queue();
            }

            return;
        }

        auto &q = port_byte_queue[port];

        if (q.full()) {
            ui::rx_error(port);
            ++pstat.rcv_ovf;

            return;
        }

        if (q.empty() && !is_cmd && (port_queue_mask == 0 || port_queue.front() != port)) {
            ui::rx_error(port);
            ++pstat.rcv_cmd;

            return;
        }

        port_queue_add(port);
        q.push_back(data);

        if (dre_wait_mask == 0 && port_queue.front() == port) {
            process_queue();

            return;
        }

        if (last_byte_time != 0 && !pending_timer.active()) {
            pending_timer.schedule(last_byte_time + PENDING_TIMEOUT_MS);
        }
    }

    void process_dre(uint8_t port) {
        dre_wait_mask &= ~(1 << port);

        if (dre_wait_mask == 0) {
            process_queue();
        }
    }

    void pending_timeout() {
        if (dre_wait_mask != 0) {
            return;
        }

        uint8_t port = port_queue_pop();

        ui::rx_error(port);
        ++port_stat[port].rcv_to;

        process_queue();
    }

    void dump() {
        cdc_println("drew: ", dre_wait_mask);
        cdc_println("bp:  ", pending_bytes);
        cdc_println("pq(m/s/f): ", port_queue_mask, " ", port_queue.size(), " ", port_queue.front());
    }

private:
    void send(uint8_t data) {
        UL::write_byte(data);

        if (!UL::tx_ring_empty()) {
            dre_wait_mask = ALL_PORTS_MASK;
        }

        if (is_midi_cmd(data)) {
            if (data == CMD_SYS_ACTIVE_S) {
                ui::tx_active();
            } else {
                ui::tx_blink();
            }
        }
    }

    inline void port_queue_add(uint8_t port) {
        if ((port_queue_mask & (1 << port)) == 0) {
            port_queue_mask |= (1 << port);

            port_queue.push_back(port);
        }
    }

    inline uint8_t port_queue_pop() {
        uint8_t port = port_queue.pop_front();
        if (port_byte_queue[port].empty()) {
            port_queue_mask &= ~(1 << port);
        } else {
            port_queue.push_back(port);
        }
        return port;
    }

    void port_queue_clear() {
        port_queue_mask = 0;
        port_queue.clear();
    }

    void process_queue() {
        uint8_t data;

        if (!rt_queue.empty()) {
            data = rt_queue.pop_front();
        } else {
            if (port_queue.empty()) {
                return false;
            }

            uint8_t port = port_queue.front();
            auto &q = port_byte_queue[port];

            if (q.empty()) {
                return false;
            }

            data = q.pop_front();

            if (update_pending_bytes(data) == 0) {
                port_queue_pop();
            }
        }

        send(data);
    }

    inline uint8_t update_pending_bytes(uint8_t data) {
        uint8_t s = midi_cmd_t::cmd_size(data);
        if (s == 0) {
            if (pending_bytes != 0 && pending_bytes != 0xFF) {
                --pending_bytes;
            }
        } else {
            if (data != CMD_SYS_EX) {
                pending_bytes = s - 1;
            } else {
                pending_bytes = 0xFF;
            }
        }

        if (pending_bytes != 0) {
            last_byte_time = millis_();

            if (port_queue.size() > 1) {
                pending_timer.schedule(last_byte_time + PENDING_TIMEOUT_MS);
            }
        } else {
            last_byte_time = 0;
            pending_timer.cancel();
        }

        return pending_bytes;
    }

    uint8_t dre_wait_mask = 0;

    ring<MIDI_PORTS + 2, uint8_t> port_queue;
    uint8_t port_queue_mask = 0;

    uint8_t pending_bytes = 0;
    unsigned long last_byte_time = 0;

    ring<16, uint8_t> port_byte_queue[MIDI_PORTS + 1];
    ring<8, uint8_t> rt_queue;
};

}
