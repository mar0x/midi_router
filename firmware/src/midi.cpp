
#include "midi.h"
#include "cdc_print.h"
#include "timer.h"
#include "midi_cmd.h"
#include "midi_normalizer.h"

namespace {
uint8_t last_port = 0xFF;
bool last_dir = false;

const char dir_str[] = {'o', 'i'};
}

namespace midi {

volatile uint8_t rx_ready;

port_stat_t port_stat[MIDI_PORTS + 1];
bool port_stat_update = false;

bool port_mon[2][MIDI_PORTS];
bool mon_enabled = false;

void start_mon(bool out, bool in) {
    last_port = 0xFF;
    for (uint8_t i = 0; i < MIDI_PORTS; ++i) {
        port_mon[0][i] = out;
        port_mon[1][i] = in;
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

normalizer_t in_port_normalizer[MIDI_PORTS];

uint8_t normalize(uint8_t port, uint8_t data) {
    uint8_t ds = midi_cmd_serial(data);

    return in_port_normalizer[port](data, ds);
}

}
