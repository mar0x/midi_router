
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
using MERGE_SRC = uart_list<uart_e0, uart_d0, uart_c1>;
using MERGE_DST = uart_list<uart_d0, uart_e0>;
using CC_DST_0 = uart_c0;
using CC_DST_1 = uart_c1;

midi::merger_t<MERGE_SRC, MERGE_DST> merger_state;
template<> midi::merger_state_t midi::merger_t<MERGE_SRC, MERGE_DST>::state = { };

enum {
    WAIT_CC,
    WAIT_0x14,
    WAIT_DATA
} port2_state = WAIT_CC;

enum {
    WAIT_COMMAND,
    WAIT_NOTE_ON_TONE,
    WAIT_NOTE_ON_VELOCITY,
    WAIT_NOTE_OFF_TONE,
    WAIT_NOTE_OFF_VELOCITY,
} port3_state = WAIT_COMMAND;

uint8_t port3_last_cmd;
uint8_t port3_last_tone;
uint8_t port3_last_dst;

enum {
    NOTE_START = NOTE_C3, // C3
    NOTE_END = NOTE_Cs5,  // C#5

    FALLBACK_CC = 12,

    NOTE_VEL_DEC = 10,
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

void merger_rx_complete(uint8_t port, uint8_t data, bool ferr) {
    midi::mon(port, true, &data, 1);

    if (port == 3) {
        if (is_midi_cmd(data)) {
            ui::rx_data(data == CMD_SYS_ACTIVE_S, port);
        }

        if (!is_midi_rt(data)) {
            if (is_midi_cmd(data)) {
                port3_state = WAIT_COMMAND;
            }

            switch (port3_state) {
            case WAIT_COMMAND: {
                uint8_t cmd = midi_cmd_t::command(data);
                uint8_t ch = midi_cmd_t::channel(data);

                if (ch == 14 || ch == 15) {
                    if (cmd == CMD_NOTE_ON) {
                        port3_state = WAIT_NOTE_ON_TONE;
                        data = CMD_CTRL_CHANGE | (ch - 12);
                        port3_last_cmd = data;
                        port3_last_dst = ch - 14;
                        return;
                    }
                    if (cmd == CMD_NOTE_OFF) {
                        port3_state = WAIT_NOTE_OFF_TONE;
                        return;
                    }
                }
                break;
            }
            case WAIT_NOTE_ON_TONE:
                port3_state = WAIT_NOTE_ON_VELOCITY;
                if (data >= NOTE_START && data < NOTE_END) {
                    port3_last_tone = data;
                    data = note2cc_map[data - NOTE_START];
                } else {
                    port3_last_tone = 0xFF;
                    data = FALLBACK_CC;
                }
                return;
            case WAIT_NOTE_ON_VELOCITY:
                port3_state = WAIT_NOTE_ON_TONE;
                if (data == 0 || port3_last_tone == 0xFF) {
                    return;
                }

                data = (data < NOTE_VEL_DEC) ? 0 : data - NOTE_VEL_DEC;

                if (port3_last_dst == 0) {
                    CC_DST_0::write_byte(port3_last_cmd);
                    CC_DST_0::write_byte(note2cc_map[port3_last_tone - NOTE_START]);
                    CC_DST_0::write_byte(data);
                    ui::tx_blink(CC_DST_0::tx_traits::id);
                } else {
                    CC_DST_1::write_byte(port3_last_cmd);
                    CC_DST_1::write_byte(note2cc_map[port3_last_tone - NOTE_START]);
                    CC_DST_1::write_byte(data);
                    ui::tx_blink(CC_DST_1::tx_traits::id);
                }

                for (uint8_t i = 0; i < MAX_COMPLEX; ++i) {
                    if (note2ccs_map[i].tone == port3_last_tone) {
                        for (uint8_t n = 1; n < 4; ++n) {
                            uint8_t cc = note2ccs_map[i].cc[n];
                            if (cc == 0) {
                                break;
                            }
                            if (port3_last_dst == 0) {
                                CC_DST_0::write_byte(port3_last_cmd);
                                CC_DST_0::write_byte(cc);
                                CC_DST_0::write_byte(data);
                            } else {
                                CC_DST_1::write_byte(port3_last_cmd);
                                CC_DST_1::write_byte(cc);
                                CC_DST_1::write_byte(data);
                            }
                        }
                        return;
                    }
                }
                return;
            case WAIT_NOTE_OFF_TONE:
                port3_state = WAIT_NOTE_OFF_VELOCITY;
                return;
            case WAIT_NOTE_OFF_VELOCITY:
                port3_state = WAIT_NOTE_OFF_TONE;
                return;
            }
        }

        CC_DST_0::write_byte(data);
        CC_DST_1::write_byte(data);
        if (is_midi_cmd(data)) {
            ui::tx_data(data == CMD_SYS_ACTIVE_S, CC_DST_0::tx_traits::id);
            ui::tx_data(data == CMD_SYS_ACTIVE_S, CC_DST_1::tx_traits::id);
        }
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
    if (port > 1) {
        merger_state.process_dre(port);
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

        on_rx_complete = cb;
        on_dre = dummy_process_dre;
    } else {
        merger_state.enable();

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
