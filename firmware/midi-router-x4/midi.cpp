
#include <midi.h>
#include <uart.h>
#include <crit_sec.h>
#include <splitter.h>
#include <merger.h>

namespace {

using uart_c0 = uart_t<port::C0, 31250, rx_midi_traits<2>, tx_midi_traits<0> >;
template<> uart_c0::tx_ring_t uart_c0::tx_ring = {};
template<> uint8_t uart_c0::want_write = 0;

using uart_c1 = uart_t<port::C1, 31250, rx_midi_traits<3>, tx_midi_traits<1> >;
template<> uart_c1::tx_ring_t uart_c1::tx_ring = {};
template<> uint8_t uart_c1::want_write = 0;

using uart_d0 = uart_t<port::D0, 31250, rx_midi_traits<0>, tx_midi_traits<2> >;
template<> uart_d0::tx_ring_t uart_d0::tx_ring = {};
template<> uint8_t uart_d0::want_write = 0;

using uart_e0 = uart_t<port::E0, 31250, rx_midi_traits<1>, tx_midi_traits<3> >;
template<> uart_e0::tx_ring_t uart_e0::tx_ring = {};
template<> uint8_t uart_e0::want_write = 0;

using ALL = uart_list<uart_c0, uart_c1, uart_d0, uart_e0>;
using MERGE_SRC = uart_list<uart_d0, uart_e0, uart_c0>;
using MERGE_DST = uart_list<uart_c0>;
using SPLIT_SRC = uart_c1;
using SPLIT_DST = uart_list<uart_c1, uart_d0, uart_e0>;

midi::merger_t<MERGE_SRC, MERGE_DST> merger_state;
template<> midi::merger_state_t midi::merger_t<MERGE_SRC, MERGE_DST>::state = { };

midi::splitter_t<SPLIT_SRC, SPLIT_DST> splitter_state;
template<> midi::splitter_state_t midi::splitter_t<SPLIT_SRC, SPLIT_DST>::state = { };

enum {
    WAIT_CC,
    WAIT_0x14,
    WAIT_DATA
} port2_state = WAIT_CC;

bool port0_active = false;
artl::timer<> port0_active_timer;

void merger_rx_complete(uint8_t port, uint8_t data, bool ferr) {
    midi::mon(port, true, &data, 1);

    if (port == 3) {
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
                if (port0_active != (data != 0)) {
                    port0_active = (data != 0);
                    if (port0_active) {
                        ui::rx_active(0);
                        port0_active_timer.schedule(millis() + 300);
                    } else {
                        ui::rx_blink_state[0].stop();
                        port0_active_timer.cancel();
                    }
                }
                port2_state = WAIT_CC;
                break;
            }
        }
        if (port == 0 && !port0_active) {
            return;
        }

        merger_state.rx_complete(port, data, ferr);
    }
}

void merger_process_dre(uint8_t port) {
    if (port == 3) {
        splitter_state.process_dre(port);
    } else {
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

    if (port0_active_timer.update(t)) {
        ui::rx_active(0);
        port0_active_timer.schedule(t + 300);
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
