
#include <midi.h>
#include <uart.h>
#include <crit_sec.h>
#include <splitter.h>

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

using UL = uart_list<uart_c0, uart_c1, uart_d0, uart_e0>;

midi::splitter_t<UL> splitter_state;
template<> midi::splitter_state_t midi::splitter_t<UL>::state = { };

void splitter_rx_complete(uint8_t port, uint8_t data, bool ferr) {
    splitter_state.rx_complete(port, data, ferr);
}

void splitter_process_dre(uint8_t port) {
    splitter_state.process_dre(port);
}

template<typename T>
inline void rx_complete() {
    crit_sec cs;
    bool ferr = T::ferr();
    midi::on_rx_complete(T::rx_traits::id, T::data(), ferr);
}

template<typename T>
inline void process_bit() {
    crit_sec cs;
    splitter_state.process_bit(T::rx_traits::id, T::rx::read());
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
    if (cb) {
        splitter_state.disable();

        on_rx_complete = cb;
        on_dre = dummy_process_dre;
    } else {
        splitter_state.enable();

        on_rx_complete = splitter_rx_complete;
        on_dre = splitter_process_dre;
    }
}

uint8_t send(uint8_t port, const uint8_t *buf, uint8_t size) {
    return UL::write_buf(port, buf, size);
}

void pending_timeout() {
    crit_sec cs;
    splitter_state.pending_timeout();
}

void dump_state() {
    crit_sec cs;
    splitter_state.dump();
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
