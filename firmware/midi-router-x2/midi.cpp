
#include <midi.h>
#include <uart.h>
#include <crit_sec.h>
#include <midi_mixer.h>

namespace {

using uart_c0 = uart_t<port::C0, 31250, rx_midi_traits<0>, tx_midi_traits<0> >;
template<> uart_c0::tx_ring_t uart_c0::tx_ring = {};

using uart_c1 = uart_t<port::C1, 31250, rx_midi_traits<1>, tx_midi_traits<1> >;
template<> uart_c1::tx_ring_t uart_c1::tx_ring = {};

using ALL = uart_list<uart_c0, uart_c1>;

template<typename T>
inline void process_rxc() {
    crit_sec cs;
    bool ferr = T::ferr();
    midi::process_rxc(T::rx_traits::id, T::data(), ferr);
}

template<typename T>
inline void process_dre() {
    crit_sec cs;
    T::on_dre_int();
    midi::mixer[T::tx_traits::id].on_dre_int();
}

}

namespace midi {

void setup() {
    ALL::setup();
    ALL::rxc_int_hi();

    setup_mixer<uart_c0>();
    setup_mixer<uart_c1>();
}

}


ISR(USARTC0_RXC_vect)
{
    process_rxc<uart_c0>();
}

ISR(USARTC0_DRE_vect)
{
    process_dre<uart_c0>();
}


ISR(USARTC1_RXC_vect)
{
    process_rxc<uart_c1>();
}

ISR(USARTC1_DRE_vect)
{
    process_dre<uart_c1>();
}
