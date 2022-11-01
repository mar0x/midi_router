
extern "C" {
#include "asf.h"
#include <conf_usb.h>
}

#include "usb.h"
#include "usb_midi_buf.h"
#include "midi.h"
#include "ui.h"
#include "timer.h"

namespace {

usb_midi_buf_t rx_buf;
bool wait_rx_complete = false;

usb_midi_buf_t tx_buf[2];
uint8_t tx_active_buf = 0;
bool wait_tx_complete = false;

void flush();

void tx_complete(udd_ep_status_t status, iram_size_t n, udd_ep_id_t ep) {
    if (status == UDD_EP_TRANSFER_OK) {
        midi::port_stat[MIDI_PORTS].snd_msgs += n / sizeof(usb_midi_event_t);
        midi::port_stat[MIDI_PORTS].snd_bytes += n;
    } else {
        ++midi::port_stat[MIDI_PORTS].stall_msgs;
    }

    wait_tx_complete = false;

    if (!tx_buf[tx_active_buf].empty()) {
        flush();
    }
}

void flush() {
    uint8_t *buf = tx_buf[tx_active_buf];
    uint8_t size = tx_buf[tx_active_buf].size();

    tx_active_buf = (tx_active_buf + 1) % 2;
    tx_buf[tx_active_buf].reset();

    wait_tx_complete = true;

    if (udd_ep_run(UDI_MIDI_EP_IN, true, buf, size, tx_complete)) {

    } else {
        ++midi::port_stat[MIDI_PORTS].stall_msgs;
        wait_tx_complete = false;
    }
}

void rx_complete(udd_ep_status_t status, iram_size_t n, udd_ep_id_t ep) {
    wait_rx_complete = false;

    rx_buf.received(n);

    midi::port_stat[MIDI_PORTS].rcv_bytes += n;
    midi::port_stat[MIDI_PORTS].rcv_msgs += n / sizeof(usb_midi_event_t);

    usb::process_rx();
}

void wait_rx() {
    if (!wait_rx_complete) {
        wait_rx_complete = true;

        udd_ep_run(UDI_MIDI_EP_OUT,
            true,
            rx_buf,
            rx_buf.capacity(),
            rx_complete);
    }
}

inline void copy(usb_midi_event_t& ev, const midi_cmd_t& c, uint8_t jack, uint8_t ds) {
    if (ds != CMDID_SYS_EX) {
        ev.header = c.cmdid2hdr(ds);
    } else {
        if (c.size() == 3) {
            ev.header = (c[2] == CMD_SYS_EX_END) ? 0x07 : 0x04;
        } else {
            ev.header = 0x06;
        }
    }

    ev.header |= (jack << 4);
    ev.byte1 = c[0];
    ev.byte2 = c.size() > 1 ? c[1] : 0;
    ev.byte3 = c.size() > 2 ? c[2] : 0;
}

bool midi_send(const usb_midi_event_t &ev, uint8_t jack, bool stall) {
    uint8_t s = ev.size();

    if (s == 0) return true;

    midi::port_stat_t &stat = midi::port_stat[jack];

    bool res = midi::mixer[jack]((const uint8_t *) ev, s, midi_cmd_serial(ev.byte1));
    if (!res) {
        if (stat.stall_start == 0) {
            stat.stall_start = millis();
        }

        return false;
    }

    stat.snd_bytes += s;
    ++stat.snd_msgs;

    midi::port_stat_update = true;

    if (stall != 0) {
        stat.stall_bytes += s;
        ++stat.stall_msgs;
    }

    if (stat.stall_start != 0) {
        stat.stall_ms += millis() - stat.stall_start;

        stat.stall_start = 0;
    }

    //midi::mon(jack, false, ev, ev.size());

    if (ev.byte1 == CMD_SYS_ACTIVE_S) {
        ui::rx_usb_active();
        ui::tx_active(jack);
    } else {
        ui::rx_usb_blink();
        ui::tx_blink(jack);
    }

    return true;
}

}

namespace usb {

bool send(uint8_t jack, const midi_cmd_t& c, uint8_t ds)
{
    if (tx_buf[tx_active_buf].full()) {
        ++midi::port_stat[MIDI_PORTS].snd_ovf;
        return false;
    }

    usb_midi_event_t &ev = tx_buf[tx_active_buf].push();

    copy(ev, c, jack, ds);

    if (c.sys_active_state()) {
        ui::tx_usb_active();
    } else {
        ui::tx_usb_blink();
    }

    if (!wait_tx_complete) {
        flush();
    }

    return true;
}

bool process_rx() {
    while (!rx_buf.empty()) {
        const usb_midi_event_t& ev = rx_buf.first();
        if (midi_send(ev, ev.jack(), rx_buf.stall)) {
            rx_buf.pop();
        } else {
            rx_buf.set_stall();
            return false;
        }
    }

    wait_rx();

    return true;
}

void enable() {
    wait_rx();
}

void disable() {
}

}
