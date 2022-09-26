
#include "midi.h"
#include "cdc_print.h"
#include "timer.h"
#include "midi_cmd.h"

namespace {
uint8_t last_port = 0xFF;
bool last_dir = false;

char dir_str[] = {'o', 'i'};
}

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

bool port_mon[2][MIDI_PORTS];
bool mon_enabled = false;

artl::timer<> pending_timer;

process_byte_t on_rx_complete;
process_dre_t on_dre;

void start_mon() {
    last_port = 0xFF;
    for (uint8_t i = 0; i < MIDI_PORTS; ++i) {
        port_mon[0][i] = true;
        port_mon[1][i] = true;
    }
    mon_enabled = true;
}

void stop_mon() {
    for (uint8_t i = 0; i < MIDI_PORTS; ++i) {
        port_mon[0][i] = false;
        port_mon[1][i] = false;
    }
    mon_enabled = false;
}

void dummy_process_dre(uint8_t port) { }

void mon(uint8_t port, bool dir_in, const uint8_t *b, uint8_t size) {
    if (cdc_dtr && port_mon[dir_in][port]) {
        iram_size_t csize = udi_cdc_get_free_tx_buffer();
        iram_size_t psize = 3 * size;
        bool c = (port == last_port && dir_in == last_dir && (*b == CMD_SYS_EX_END || !is_midi_cmd(*b)));

        if (!c) {
            psize += 10 + 3 + 2;

            last_port = port;
            last_dir = dir_in;
        }

        if (csize >= psize) {
            if (!c) {
                _cdc_print_eol();
                _cdc_print(millis(), ' ', dir_str[dir_in], port, ' ');
            }

            for (uint8_t i = 0; i < size; ++i) {
                _cdc_print_hex(b[i]);
                _cdc_print(' ');
            }

            if (b[size - 1] == CMD_SYS_EX_END) {
                last_port = 0xFF;
            }
        } else {
            if (csize >= 1) {
                _cdc_print('.');
            }
        }
    }
}

struct port_cfg_t {
    uint32_t command_filter;
    uint16_t channel_filter;

    uint8_t filter_state;

    uint8_t channel_map[8];
};

port_cfg_t in_port_cfg[MIDI_IN_PORTS];
port_cfg_t out_port_cfg[MIDI_OUT_PORTS];

uint8_t port_input_routing[MIDI_IN_PORTS];

bool input_filter(uint8_t port, uint8_t data, uint8_t ds) {
    port_cfg_t &cfg = in_port_cfg[port];

    if (ds == 0) {
        return cfg.filter_state;
    }

    bool res = (cfg.command_filter & (1 << ds)) != 0;
    if (res) {
        uint8_t ch = data & 0x0F;
        res = (cfg.channel_filter & (1 << ch)) != 0;
    }

    cfg.filter_state = res;

    return res;
}

void input_channelizer(uint8_t port, uint8_t &data, uint8_t ds) {
    if (ds == 0 || ds > 7) {
        return;
    }

    port_cfg_t &cfg = in_port_cfg[port];

    uint8_t ch = data & 0x0F;
    uint8_t cm = cfg.channel_map[ch >> 1];
    if ((ch & 0x01) == 0) {
        cm = cm >> 4;
    } else {
        cm = cm & 0x0F;
    }

    data = (data & 0xF0) | cm;
}


}
