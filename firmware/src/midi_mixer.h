#pragma once

extern "C" {
#include "asf.h"
}

#include "ring.h"
#include "midi.h"
#include "midi_cmd.h"
#include "timer.h"
#include "cdc_print.h"
#include "artl/timer.h"
#include "crit_sec.h"

void midi_send_ready(uint8_t port, uint8_t size);

namespace midi {

using write_byte_get_dre_f = bool (*)(uint8_t data);

struct mixer_t {
    enum {
        PENDING_TIMEOUT_MS = 4,
    };

    enum {
        USB_IN_PORT = MIDI_IN_PORTS,
        RT_QUEUE = 0xFF,
        UWW_NONE = 0xFE,
    };

    static bool dummy_write(uint8_t date) { return false; }

    mixer_t() : write_cb(dummy_write) { }

    void setup(uint8_t n, write_byte_get_dre_f cb) {
        id = n;
        write_cb = cb;
        reset();
    }

    void reset() {
        memset(&reset_start, 0, reset_end - reset_start);
        uww_port = UWW_NONE;
    }

    bool operator()(uint8_t src, uint8_t data, uint8_t ds) {
        if (ds >= CMDID_SYS_RT) {
            bool ovf = rt_queue.full();

            if (ovf) {
                rt_queue.pop_front();
            }

            rt_queue.push_back(data);

            if (!dre_wait) {
                process_queue();
            }

            return !ovf;
        }

        if (!data_queue_add(src, data)) {
            return false;
        }

        port_queue_add(src);

        return true;
    }

    bool operator()(const uint8_t *data, uint8_t size, uint8_t ds) {
        crit_sec cs;

        if (ds >= CMDID_SYS_RT) {
            if (rt_queue.full()) {
                usb_want_write += 1;
                uww_port = RT_QUEUE;
                return false;
            }

            rt_queue.push_back(*data);

            if (!dre_wait) {
                process_queue();
            }

            return true;
        }

        if (!usb_data_queue_push(data, size)) {
            return false;
        }

        port_queue_add(USB_IN_PORT);

        return true;
    }

    void on_dre_int() {
        dre_wait = false;

        if (pending_timeout_fired) {
            pending_timeout_fired = false;

            if (port_queue_front_is(pending_port)) {
                port_queue_pop();
            }
        }

        process_queue();
    }

    bool update_timer(unsigned long t, uint8_t &port) {
        if (pending_timer.update(t)) {
            pending_start_time = 0;

            if (!port_queue_front_is(pending_port)) {
                return false;
            }

            port = pending_port;

            dump_queues(millis_(), pending_port);

            if (dre_wait) {
                pending_timeout_fired = true;
            } else {
                port_queue_pop();

                process_queue();
            }

            return true;
        }

        return false;
    }

    void dump() {
        iram_size_t csize = udi_cdc_get_free_tx_buffer();

        if (csize >= (iram_size_t) (4 + 2 + 4 + 2 + 11 + 2 + 2 + 2 + 2)) {
            _cdc_print("dre:", (uint8_t) dre_wait);
            _cdc_print(",pb:", pending_bytes, ",pq(m/s/f):");
            _cdc_print_hex(port_queue_mask);
            _cdc_println("/", port_queue.size(), "/", port_queue.front());
        }
    }

private:
    void process_queue() {
        do {
            uint8_t data, src;

            if (!data_queue_pop(data, src)) {
                return;
            }

            dre_wait = write_cb(data);

            if (src != RT_QUEUE && update_pending_bytes(data) == 0) {
                port_queue_pop();
            }

            if (src == uww_port && usb_want_write > 0) {
                uint8_t avail = (src == RT_QUEUE) ?
                    rt_queue.avail() : usb_data_queue.avail();

                if (avail >= usb_want_write) {
                    usb_want_write = 0;
                    uww_port = UWW_NONE;
                    midi_send_ready(id, avail);
                }
            }

        } while(!dre_wait);
    }

    bool data_queue_pop(uint8_t &data, uint8_t &src) {
        if (!rt_queue.empty()) {
            data = rt_queue.pop_front();
            src = RT_QUEUE;
            return true;
        }

        if (port_queue_empty()) {
            return false;
        }

        src = port_queue_front();

        if (src == USB_IN_PORT) {
            if (usb_data_queue.empty()) {
                return false;
            }

            data = usb_data_queue.pop_front();

            return true;
            // return usb_data_queue.pop_front(data);
        } else {
            return port_data_queue[src].pop_front(data);
        }
    }

    uint8_t update_pending_bytes(uint8_t data) {
        uint8_t s = midi_cmd_t::cmd_size(data);
        if (s != 0) {
            pending_bytes = s;
        }
        if (pending_bytes != 0 && pending_bytes != 0xFF) {
            --pending_bytes;
        }

        if (pending_bytes != 0) {
            pending_start_time = millis_();

            if (port_queue.size() > 1) {
                schedule_pending_timer();
            }
        } else {
            if (pending_start_time != 0) {
                pending_start_time = 0;
                pending_timer.cancel();
            }
        }

        return pending_bytes;
    }

    void schedule_pending_timer() {
        pending_port = port_queue_front();
        pending_timer.schedule(pending_start_time + PENDING_TIMEOUT_MS);
    }

    void port_queue_add(uint8_t src) {
        uint8_t src_mask = (1 << src);
        if ((port_queue_mask & src_mask) == 0) {
            port_queue_mask |= src_mask;

            port_queue.push_back(src);
        }

        if (!dre_wait && port_queue_front() == src) {
            process_queue();
        } else {
            if (pending_start_time != 0 &&
                port_queue.size() > 1 &&
                !pending_timer.active()) {
                schedule_pending_timer();
            }
        }
    }

    uint8_t port_queue_pop() {
        uint8_t port = port_queue.pop_front();
        bool empty = port == USB_IN_PORT ?
            usb_data_queue.empty() :
            port_data_queue[port].empty();
        if (empty) {
            port_queue_mask &= ~(1 << port);
        } else {
            port_queue.push_back(port);
        }
        return port;
    }

    bool port_queue_empty() const { return port_queue_mask == 0; }
    uint8_t port_queue_front() const { return port_queue.front(); }
    bool port_queue_front_is(uint8_t port) const {
        return port_queue_mask != 0 && port == port_queue.front();
    }

    bool data_queue_add(uint8_t port, uint8_t data) {
        auto &q = port_data_queue[port];

        if (q.full()) {
            return false;
        }

        q.push_back(data);
        return true;
    }

    bool usb_data_queue_push(const uint8_t *data, uint8_t size) {
        if (usb_data_queue.avail() < size) {
            usb_want_write += size;
            uww_port = USB_IN_PORT;
            return false;
        }

        for (uint8_t i = 0; i < size; ++i) {
            usb_data_queue.push_back(data[i]);
        }
        return true;
    }

    void dump_queues(unsigned long t, uint8_t port) {
        if (!cdc_dtr) return;

        if (t - last_dump_time < 1000) {
            return;
        }
        last_dump_time = t;

        iram_size_t csize = udi_cdc_get_free_tx_buffer();

        static const iram_size_t port_queue_dump_size = 2 + 2 * port_queue.capacity + 2;

        if (csize >= port_queue_dump_size) {
            _cdc_print(port_queue.size(), ":");
            for(uint8_t i = 0; i < port_queue.capacity; ++i) {
                _cdc_print(port_queue.data[i],
                    i == port_queue.begin ? "<" : " ");
            }
            _cdc_print_eol();
            csize -= port_queue_dump_size;
        }

        static const iram_size_t port_data_queue_dump_size = 2 + 3 + 3 * port_data_queue[0].capacity + 2;

        for (uint8_t p = 0; p < MIDI_IN_PORTS; ++p) {
            if (csize < port_data_queue_dump_size) break;

            auto &q = port_data_queue[p];

            _cdc_print(p, p == port ? "*" : " ");
            _cdc_print_w(q.size(), 2);
            _cdc_print(":");

            for (uint8_t i = 0; i < q.capacity; ++i) {
                _cdc_print_hex(q.data[i]);
                _cdc_print(i == q.begin ? "<" : " ");
            }
            _cdc_print_eol();

            csize -= port_data_queue_dump_size;
        }

        dump();
    }

    uint8_t id;
    write_byte_get_dre_f write_cb;

    union {
        uint8_t reset_start;
        bool dre_wait;
    };

    ring<MIDI_IN_PORTS + 1, uint8_t> port_queue;
    uint8_t port_queue_mask;

    uint8_t pending_bytes;
    uint8_t pending_port;
    unsigned long pending_start_time;
    artl::timer<> pending_timer;
    bool pending_timeout_fired;

    ring<15, uint8_t> port_data_queue[MIDI_IN_PORTS];
    ring<31, uint8_t> usb_data_queue;
    ring<7, uint8_t> rt_queue;

    unsigned long last_dump_time;

    uint8_t usb_want_write;

    union {
        uint8_t uww_port;
        uint8_t reset_end;
    };
};

}
