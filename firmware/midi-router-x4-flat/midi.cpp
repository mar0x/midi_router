
#include <midi.h>
#include <uart.h>
#include <crit_sec.h>
#include <splitter.h>
#include <merger.h>

namespace {

using uart_c0 = uart_t<port::C0, 31250, rx_midi_traits<3>, tx_midi_traits<0> >;
template<> uart_c0::tx_ring_t uart_c0::tx_ring = {};
template<> uint8_t uart_c0::want_write = 0;

using uart_c1 = uart_t<port::C1, 31250, rx_midi_traits<2>, tx_midi_traits<1> >;
template<> uart_c1::tx_ring_t uart_c1::tx_ring = {};
template<> uint8_t uart_c1::want_write = 0;

using uart_d0 = uart_t<port::D0, 31250, rx_midi_traits<1>, tx_midi_traits<2> >;
template<> uart_d0::tx_ring_t uart_d0::tx_ring = {};
template<> uint8_t uart_d0::want_write = 0;

using uart_e0 = uart_t<port::E0, 31250, rx_midi_traits<0>, tx_midi_traits<3> >;
template<> uart_e0::tx_ring_t uart_e0::tx_ring = {};
template<> uint8_t uart_e0::want_write = 0;

using ALL = uart_list<uart_c0, uart_c1, uart_d0, uart_e0>;
using MERGE_SRC = uart_list<uart_d0, uart_c1, uart_c0>;
using MERGE_DST_0 = uart_list<uart_c0>;
using MERGE_DST_1 = uart_list<uart_c1>;
using SPLITTER_SRC = uart_list<uart_e0>;
using SPLITTER_DST = uart_list<uart_d0, uart_e0>;

midi::merger_t<MERGE_SRC, MERGE_DST_0> merger_state_0;
template<> midi::merger_state_t midi::merger_t<MERGE_SRC, MERGE_DST_0>::state = { };

midi::merger_t<MERGE_SRC, MERGE_DST_1> merger_state_1;
template<> midi::merger_state_t midi::merger_t<MERGE_SRC, MERGE_DST_1>::state = { };

midi::splitter_t<SPLITTER_SRC, SPLITTER_DST> splitter_state;
template<> midi::splitter_state_t midi::splitter_t<SPLITTER_SRC, SPLITTER_DST>::state = { };

enum {
    WAIT_COMMAND,
    WAIT_NOTE_ON_TONE,
    WAIT_NOTE_ON_VELOCITY,
    WAIT_NOTE_OFF_TONE,
    WAIT_NOTE_OFF_VELOCITY,
    WAIT_CTRL_ID,
    WAIT_CTRL_VALUE,
} port_state[MIDI_PORTS] = { WAIT_COMMAND, WAIT_COMMAND, WAIT_COMMAND, WAIT_COMMAND };

uint8_t port_last_tone[MIDI_PORTS];
uint8_t port_last_dst[MIDI_PORTS];
uint8_t port_last_cc[MIDI_PORTS];
uint8_t last_bio_cc_id = 0;
uint8_t last_bio_cc_value = 0;

enum {
    NOTE_START = NOTE_C3, // C3
    NOTE_END = NOTE_Cs5,  // C#5

    FALLBACK_CC = 12,

    NOTE_VEL_DEC = 10,

    BIO_DATA_PORT = 1,
    BIO_DATA_CH = 0,
    BIO_DATA_DST = 0xFF,
    BIO_DATA_CC = 80,
};

struct complex_map {
    uint8_t tone;
    uint8_t cc[4];
};

complex_map note2ccs_map[] = {
    /* C3  */ { NOTE_C3,  { 20, 21, 22, 23 }, },
    /* C#3 */ { NOTE_Cs3, { 24, 25, 26, 27 }, },
    /* D3  */ { NOTE_D3,  { 28, 29, 30, 31 }, },

    /* E4  */ { NOTE_E4,  { 102, 103, 104, 0 }, },
    /* F4  */ { NOTE_F4,  { 105, 106, 107, 0 }, },
    /* F#4 */ { NOTE_Fs4, { 108, 109, 0, 0 }, },
};

enum {
    MAX_COMPLEX = sizeof(note2ccs_map) / sizeof(note2ccs_map[0]),
};

uint8_t note2cc_map[] = {
    /* C3  */  20, // 21, 22, 23
    /* C#3 */  24, // 25, 26, 27
    /* D3  */  28, // 29, 30, 31
    /* D#3 */  52,
    /* E3  */  53,
    /* F3  */  54,
    /* F#3 */  61,
    /* G3  */  55,
    /* G#3 */  56,
    /* A3  */  57,
    /* A#3 */  58,
    /* B3  */  13,
    /* C4  */  75,
    /* C#4 */  76,
    /* D4  */  77,
    /* D#4 */  78,
    /* E4  */ 102, // 103, 104
    /* F4  */ 105, // 106, 107
    /* F#4 */ 108, // 109
    /* G4  */  62,
    /* G#4 */  40,
    /* A4  */  41,
    /* A#4 */  42,
    /* B4  */  43,
    /* C5  */  59,
};

void merger_send_0(uint8_t port, uint8_t cmd, uint8_t cc, uint8_t data) {
    merger_state_0.rx_complete(port, cmd, false);
    merger_state_0.rx_complete(port, cc, false);
    merger_state_0.rx_complete(port, data, false);
}

void merger_send_1(uint8_t port, uint8_t cmd, uint8_t cc, uint8_t data) {
    merger_state_1.rx_complete(port, cmd, false);
    merger_state_1.rx_complete(port, cc, false);
    merger_state_1.rx_complete(port, data, false);
}

void merger_rx_complete(uint8_t port, uint8_t data, bool ferr) {
    midi::mon(port, true, &data, 1);

    if (port == 0) {
        splitter_state.rx_complete(port, data, ferr);
        return;
    }

    bool rt = false;
    bool sys = false;
    if (is_midi_cmd(data)) {
        ui::rx_data(data == CMD_SYS_ACTIVE_S, port);

        rt = is_midi_rt(data);
        sys = is_midi_sys(data);
    }

    if (sys) {
        if (!rt) {
            port_state[port] = WAIT_COMMAND;
        }

        merger_state_0.rx_complete(port, data, ferr);
        merger_state_1.rx_complete(port, data, ferr);
        return;
    }

    switch (port_state[port]) {
    case WAIT_COMMAND: {
        uint8_t cmd = midi_cmd_t::command(data);
        uint8_t ch = midi_cmd_t::channel(data);

        if (port == BIO_DATA_PORT && ch == BIO_DATA_CH) {
            switch (cmd) {
            case CMD_NOTE_ON:
                port_state[port] = WAIT_NOTE_ON_TONE;
                port_last_dst[port] = BIO_DATA_DST;
                break;
            case CMD_NOTE_OFF:
                port_state[port] = WAIT_NOTE_OFF_TONE;
                break;
            case CMD_CTRL_CHANGE:
                port_state[port] = WAIT_CTRL_ID;
                port_last_dst[port] = BIO_DATA_DST;
                break;
            }
            return;
        }

        if (ch == 14 || ch == 15) {
            switch (cmd) {
            case CMD_NOTE_ON:
                port_state[port] = WAIT_NOTE_ON_TONE;
                port_last_dst[port] = ch - 14;
                return;
            case CMD_NOTE_OFF:
                port_state[port] = WAIT_NOTE_OFF_TONE;
                return;
            case CMD_CTRL_CHANGE:
                port_state[port] = WAIT_CTRL_ID;
                port_last_dst[port] = ch - 14;
                break;
            }
        }
        break;
    }
    case WAIT_NOTE_ON_TONE:
        port_state[port] = WAIT_NOTE_ON_VELOCITY;
        if (data >= NOTE_START && data < NOTE_END) {
            port_last_tone[port] = data;
            port_last_cc[port] = note2cc_map[data - NOTE_START];
        } else {
            port_last_tone[port] = 0xFF;
        }
        return;
    case WAIT_NOTE_ON_VELOCITY:
        port_state[port] = WAIT_NOTE_ON_TONE;
        if (data == 0 || port_last_tone[port] == 0xFF) {
            return;
        }

        if (port_last_dst[port] == BIO_DATA_DST) {
            data = last_bio_cc_value;
            uint8_t cc = port_last_cc[port];

            merger_send_0(port, CMD_CTRL_CHANGE | 2, cc, data);
            merger_send_1(port, CMD_CTRL_CHANGE | 3, cc, data);
        } else {
            data = (data < NOTE_VEL_DEC) ? 0 : data - NOTE_VEL_DEC;
            uint8_t cc = port_last_cc[port];

            if (port_last_dst[port] == 0) {
                merger_send_0(port, CMD_CTRL_CHANGE | 2, cc, data);
            } else {
                merger_send_1(port, CMD_CTRL_CHANGE | 3, cc, data);
            }
        }

        for (uint8_t i = 0; i < MAX_COMPLEX; ++i) {
            if (note2ccs_map[i].tone == port_last_tone[port]) {
                for (uint8_t n = 1; n < 4; ++n) {
                    uint8_t cc = note2ccs_map[i].cc[n];
                    if (cc == 0) {
                        break;
                    }
                    if (port_last_dst[port] == BIO_DATA_DST) {
                        merger_send_0(port, CMD_CTRL_CHANGE | 2, cc, data);
                        merger_send_1(port, CMD_CTRL_CHANGE | 3, cc, data);
                    } else if (port_last_dst[port] == 0) {
                        merger_send_0(port, CMD_CTRL_CHANGE | 2, cc, data);
                    } else {
                        merger_send_1(port, CMD_CTRL_CHANGE | 3, cc, data);
                    }
                }
                return;
            }
        }
        return;
    case WAIT_NOTE_OFF_TONE:
        port_state[port] = WAIT_NOTE_OFF_VELOCITY;
        return;
    case WAIT_NOTE_OFF_VELOCITY:
        port_state[port] = WAIT_NOTE_OFF_TONE;
        return;
    case WAIT_CTRL_ID:
        port_state[port] = WAIT_CTRL_VALUE;
        if (port_last_dst[port] == BIO_DATA_DST) {
            last_bio_cc_id = data;
        }
        return;
    case WAIT_CTRL_VALUE:
        port_state[port] = WAIT_CTRL_ID;
        if (port_last_dst[port] == BIO_DATA_DST && last_bio_cc_id == BIO_DATA_CC) {
            data = (data < NOTE_VEL_DEC) ? 0 : data - NOTE_VEL_DEC;
            last_bio_cc_value = data;

            uint8_t cc = port_last_cc[port];

            if (cc != 0) {
                merger_send_0(port, CMD_CTRL_CHANGE | 2, cc, data);
                merger_send_1(port, CMD_CTRL_CHANGE | 3, cc, data);
            }
        }
        return;
    }
}

void merger_process_dre(uint8_t port) {
    if (port > 1) {
        splitter_state.process_dre(port);
    } else {
        if (port == 0) merger_state_0.process_dre(port);
        if (port == 1) merger_state_1.process_dre(port);
    }
}

template<typename T>
inline void rx_complete() {
    crit_sec cs;
    bool ferr = T::ferr();
    midi::on_rx_complete(T::rx_traits::id, T::data(), ferr);
}

template<typename T>
inline void process_bit() {
}

template<typename T>
inline void process_dre() {
    crit_sec cs;
    T::on_dre_int();

    midi::on_dre(T::tx_traits::id);
}

}

namespace midi {

void init(process_byte_t cb) {
    static bool first_time_init = true;

    if (first_time_init) {
        ALL::setup();
        ALL::rxc_int_hi();

        first_time_init = false;
    }

    if (cb) {
        merger_state_0.disable();
        merger_state_1.disable();
        splitter_state.disable();

        on_rx_complete = cb;
        on_dre = dummy_process_dre;
    } else {
        merger_state_0.enable();
        merger_state_1.enable();
        splitter_state.enable();

        on_rx_complete = merger_rx_complete;
        on_dre = merger_process_dre;
    }
}

uint8_t send(uint8_t port, const uint8_t *buf, uint8_t size) {
    return ALL::write_buf(port, buf, size);
}

void timer_update(unsigned long t) {
    crit_sec cs;
    merger_state_0.timer_update(t);
    merger_state_1.timer_update(t);
}

void dump_state() {
    crit_sec cs;
    merger_state_0.dump();
    merger_state_1.dump();
}

}


ISR(USARTC0_RXC_vect)
{
    rx_complete<uart_c0>();
}

ISR(USARTC0_DRE_vect)
{
    process_dre<uart_c0>();
}

ISR(PORTC_INT0_vect)
{
    process_bit<uart_c0>();
}


ISR(USARTC1_RXC_vect)
{
    rx_complete<uart_c1>();
}

ISR(USARTC1_DRE_vect)
{
    process_dre<uart_c1>();
}

ISR(PORTC_INT1_vect)
{
    process_bit<uart_c1>();
}


ISR(USARTD0_RXC_vect)
{
    rx_complete<uart_d0>();
}

ISR(USARTD0_DRE_vect)
{
    process_dre<uart_d0>();
}

ISR(PORTD_INT0_vect)
{
    process_bit<uart_d0>();
}


ISR(USARTE0_RXC_vect)
{
    rx_complete<uart_e0>();
}

ISR(USARTE0_DRE_vect)
{
    process_dre<uart_e0>();
}

ISR(PORTE_INT0_vect)
{
    process_bit<uart_e0>();
}
