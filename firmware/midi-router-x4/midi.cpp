
#include <midi.h>
#include <uart.h>
#include <crit_sec.h>

namespace {

using uart_c0 = uart_t<port::C0, 31250, rx_midi_traits<2>, tx_midi_traits<2> >;
template<> uart_c0::tx_ring_t uart_c0::tx_ring = {};
template<> uint8_t uart_c0::want_write = 0;

using uart_c1 = uart_t<port::C1, 31250, rx_midi_traits<3>, tx_midi_traits<3> >;
template<> uart_c1::tx_ring_t uart_c1::tx_ring = {};
template<> uint8_t uart_c1::want_write = 0;

using uart_d0 = uart_t<port::D0, 31250, rx_midi_traits<0>, tx_midi_traits<0> >;
template<> uart_d0::tx_ring_t uart_d0::tx_ring = {};
template<> uint8_t uart_d0::want_write = 0;

using uart_e0 = uart_t<port::E0, 31250, rx_midi_traits<1>, tx_midi_traits<1> >;
template<> uart_e0::tx_ring_t uart_e0::tx_ring = {};
template<> uint8_t uart_e0::want_write = 0;

}

namespace midi {

void init() {
    PORTD.INT0MASK = 0;
    PORTD.INTCTRL = 0;

    uart_c0::setup();
    uart_c1::setup();
    uart_d0::setup();
    uart_e0::setup();

    uart_c0::rxc_int_hi();
    uart_c1::rxc_int_hi();
    uart_d0::rxc_int_hi();
    uart_e0::rxc_int_hi();
}

void splitter() {
    uart_c0::disable();
    uart_c1::disable();
    uart_d0::disable();
    uart_e0::disable();

    uart_c0::port_traits::setup_pins();
    uart_c1::port_traits::setup_pins();
    uart_d0::port_traits::setup_pins();
    uart_e0::port_traits::setup_pins();

    PORTD.INT0MASK = uart_d0::port_traits::rx::traits::bit_mask;
    PORTD.INTCTRL = PORT_INT0LVL_HI_gc;
}

uint8_t send(uint8_t port, const uint8_t *buf, uint8_t size) {
    switch (port) {
    case 0: return uart_c0::write_buf(buf, size);
    case 1: return uart_c1::write_buf(buf, size);
    case 2: return uart_d0::write_buf(buf, size);
    case 3: return uart_e0::write_buf(buf, size);
    }

    return 0;
}

}


ISR(USARTC0_RXC_vect)
{
    uart_c0::on_rxc_int();
}

ISR(USARTC0_DRE_vect)
{
    uart_c0::on_dre_int();
}


ISR(USARTC1_RXC_vect)
{
    uart_c1::on_rxc_int();
}

ISR(USARTC1_DRE_vect)
{
    uart_c1::on_dre_int();
}


ISR(USARTD0_RXC_vect)
{
    uart_d0::on_rxc_int();
}

ISR(USARTD0_DRE_vect)
{
    uart_d0::on_dre_int();
}

ISR(PORTD_INT0_vect)
{
    crit_sec cs;

    bool v = uart_d0::rx::read();

    if (v) {
        uart_d0::tx::high();
        uart_e0::tx::high();
        uart_c0::tx::high();
        uart_c1::tx::high();
    } else {
        uart_d0::tx::low();
        uart_e0::tx::low();
        uart_c0::tx::low();
        uart_c1::tx::low();

        midi::rx_ready |= (1 << 0);
        midi::tx_sent |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
    }
}


ISR(USARTE0_RXC_vect)
{
    uart_e0::on_rxc_int();
}

ISR(USARTE0_DRE_vect)
{
    uart_e0::on_dre_int();
}
