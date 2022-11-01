
#include "midi.h"
#include "midi_cmd.h"
#include "cdc_print.h"
#include "timer.h"

#define HAVE_MIDI_MON 1

namespace {

uint8_t last_port = 0xFF;
bool last_dir = false;

const char dir_str[] = {'o', 'i'};

}

namespace midi {

bool port_mon[2][MIDI_PORTS];
bool mon_enabled = false;

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

void mon(uint8_t port, bool dir_in, const uint8_t *b, uint8_t size) {
#if HAVE_MIDI_MON
    if (!cdc_dtr || !port_mon[dir_in][port]) {
        return;
    }

    iram_size_t csize = udi_cdc_get_free_tx_buffer();
    iram_size_t psize = 3 * size;

    bool new_line = port != last_port || dir_in != last_dir ||
        (*b != CMD_SYS_EX_END && is_midi_cmd(*b));

    if (new_line) {
        psize += 10 + 3 + 2;

        last_port = port;
        last_dir = dir_in;
    }

    if (csize >= psize) {
        if (new_line) {
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
#endif
}

}
