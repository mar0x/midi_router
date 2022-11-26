
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
using MERGE_SRC = uart_list<uart_d0, uart_e0, uart_c0>;
using MERGE_DST = uart_list<uart_e0>;
using SPLIT_SRC = uart_c0;
using SPLIT_DST = uart_list<uart_c1, uart_d0, uart_c0>;

midi::merger_t<MERGE_SRC, MERGE_DST> merger_state;
template<> midi::merger_state_t midi::merger_t<MERGE_SRC, MERGE_DST>::state = { };

midi::splitter_t<SPLIT_SRC, SPLIT_DST> splitter_state;
template<> midi::splitter_state_t midi::splitter_t<SPLIT_SRC, SPLIT_DST>::state = { };

enum {
    WAIT_CC,
    WAIT_0x14,
    WAIT_DATA
} port2_state = WAIT_CC;

enum {
    WAIT_NOTE_ON,
    WAIT_TONE,
    WAIT_VELOCITY
} port3_state = WAIT_NOTE_ON;

uint8_t port3_last_cmd;
uint8_t port3_last_tone;

enum {
    NOTE_START = NOTE_C2, // C2
    NOTE_END = NOTE_Cs4,  // C#4

    FALLBACK_CC = 12,

    NOTE_VEL_DEC = 10,
};

struct complex_map {
    uint8_t tone;
    uint8_t cc[4];
};

complex_map note2ccs_map[] = {
    /* C2  */ { NOTE_C2,  { 20, 21, 22, 23 }, },
    /* C#2 */ { NOTE_Cs2, { 24, 25, 26, 27 }, },
    /* D2  */ { NOTE_D2,  { 28, 29, 30, 31 }, },

    /* E3  */ { NOTE_E3,  { 102, 103, 104, 0 }, },
    /* F3  */ { NOTE_F3,  { 105, 106, 107, 0 }, },
    /* F#3 */ { NOTE_Fs3, { 108, 109, 0, 0 }, },
};

enum {
    MAX_COMPLEX = sizeof(note2ccs_map) / sizeof(note2ccs_map[0]),
};

uint8_t note2cc_map[] = {
    /* C2  */  20, // 21, 22, 23
    /* C#2 */  24, // 25, 26, 27
    /* D2  */  28, // 29, 30, 31
    /* D#2 */  52,
    /* E2  */  53,
    /* F2  */  54,
    /* F#2 */  61,
    /* G2  */  55,
    /* G#2 */  56,
    /* A2  */  57,
    /* A#2 */  58,
    /* B2  */  13,
    /* C3  */  75,
    /* C#3 */  76,
    /* D3  */  77,
    /* D#3 */  78,
    /* E3  */ 102, // 103, 104
    /* F3  */ 105, // 106, 107
    /* F#3 */ 108, // 109
    /* G3  */  62,
    /* G#3 */  40,
    /* A3  */  41,
    /* A#3 */  42,
    /* B3  */  43,
    /* C4  */  59,
};

void merger_rx_complete(uint8_t port, uint8_t data, bool ferr) {
    midi::mon(port, true, &data, 1);

    if (port == 3) {
        if (!is_midi_rt(data)) {
            switch (port3_state) {
            case WAIT_NOTE_ON:
                if (midi_cmd_t::command(data) == CMD_NOTE_ON) {
                    port3_state = WAIT_TONE;
                    data = CMD_CTRL_CHANGE | (data & 0x0FU);
                    port3_last_cmd = data;
                }
                break;
            case WAIT_TONE:
                port3_state = WAIT_VELOCITY;
                if (data >= NOTE_START && data < NOTE_END) {
                    port3_last_tone = data;
                    data = note2cc_map[data - NOTE_START];
                } else {
                    port3_last_tone = 0xFF;
                    data = FALLBACK_CC;
                }
                break;
            case WAIT_VELOCITY:
                port3_state = WAIT_NOTE_ON;
                data = (data < NOTE_VEL_DEC) ? 0 : data - NOTE_VEL_DEC;
                for (uint8_t i = 0; i < MAX_COMPLEX; ++i) {
                    if (note2ccs_map[i].tone == port3_last_tone) {
                        splitter_state.rx_complete(port, data, ferr);
                        for (uint8_t n = 1; n < 4; ++n) {
                            uint8_t cc = note2ccs_map[i].cc[n];
                            if (cc == 0) {
                                break;
                            }
                            splitter_state.rx_complete(port, port3_last_cmd, false);
                            splitter_state.rx_complete(port, cc, false);
                            splitter_state.rx_complete(port, data, false);
                        }
                        return;
                    }
                }
                break;
            }
        }

        splitter_state.rx_complete(port, data, ferr);
    } else {
        if (port == 2) {
            switch (port2_state) {
            case WAIT_CC:
                if (data == CMD_CTRL_CHANGE) {
                    port2_state = WAIT_0x14;
                }
                break;
            case WAIT_0x14:
                if (data == 0x14) {
                    port2_state = WAIT_DATA;
                } else {
                    port2_state = WAIT_CC;
                }
                break;
            case WAIT_DATA:
                port2_state = WAIT_CC;
                break;
            }
        }

        merger_state.rx_complete(port, data, ferr);
    }
}

void merger_process_dre(uint8_t port) {
    if (port == 3) {
        merger_state.process_dre(port);
    } else {
        splitter_state.process_dre(port);
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
        merger_state.disable();
        splitter_state.disable();

        on_rx_complete = cb;
        on_dre = dummy_process_dre;
    } else {
        merger_state.enable();
        splitter_state.enable();

        on_rx_complete = merger_rx_complete;
        on_dre = merger_process_dre;
    }
}

uint8_t send(uint8_t port, const uint8_t *buf, uint8_t size) {
    return ALL::write_buf(port, buf, size);
}

void timer_update(unsigned long t) {
    if (pending_timer.update(t)) {
        crit_sec cs;
        merger_state.pending_timeout();
    }
}

void dump_state() {
    crit_sec cs;
    merger_state.dump();
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
