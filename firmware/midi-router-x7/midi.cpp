
#include <midi.h>
#include <uart.h>
#include <crit_sec.h>

namespace {

using uart_c0 = uart_t<port::C0, 31250, rx_midi_traits<0>, tx_midi_traits<0> >;
template<> uart_c0::tx_ring_t uart_c0::tx_ring = {};
template<> uint8_t uart_c0::want_write = 0;

using uart_c1 = uart_t<port::C1, 31250, rx_midi_traits<1>, tx_midi_traits<1> >;
template<> uart_c1::tx_ring_t uart_c1::tx_ring = {};
template<> uint8_t uart_c1::want_write = 0;

using uart_d0 = uart_t<port::D0, 31250, rx_midi_traits<2>, tx_midi_traits<2> >;
template<> uart_d0::tx_ring_t uart_d0::tx_ring = {};
template<> uint8_t uart_d0::want_write = 0;

using uart_e0 = uart_t<port::E0, 31250, rx_midi_traits<3>, tx_midi_traits<3> >;
template<> uart_e0::tx_ring_t uart_e0::tx_ring = {};
template<> uint8_t uart_e0::want_write = 0;

using uart_e1 = uart_t<port::E1, 31250, rx_midi_traits<4>, tx_midi_traits<4> >;
template<> uart_e1::tx_ring_t uart_e1::tx_ring = {};
template<> uint8_t uart_e1::want_write = 0;

using uart_f0 = uart_t<port::F0, 31250, rx_midi_traits<5>, tx_midi_traits<5> >;
template<> uart_f0::tx_ring_t uart_f0::tx_ring = {};
template<> uint8_t uart_f0::want_write = 0;

using uart_f1 = uart_t<port::F1, 31250, rx_midi_traits<6>, tx_midi_traits<6> >;
template<> uart_f1::tx_ring_t uart_f1::tx_ring = {};
template<> uint8_t uart_f1::want_write = 0;

}

namespace midi {

void init() {
    PORTC.INT0MASK = 0;
    PORTC.INTCTRL = 0;

    uart_c0::setup();
    uart_c1::setup();
    uart_d0::setup();
    uart_e0::setup();
    uart_e1::setup();
    uart_f0::setup();
    uart_f1::setup();

    uart_c0::rxc_int_hi();
    uart_c1::rxc_int_hi();
    uart_d0::rxc_int_hi();
    uart_e0::rxc_int_hi();
    uart_e1::rxc_int_hi();
    uart_f0::rxc_int_hi();
    uart_f1::rxc_int_hi();
}

void splitter() {
    uart_c0::disable();
    uart_c1::disable();
    uart_d0::disable();
    uart_e0::disable();
    uart_e1::disable();
    uart_f0::disable();
    uart_f1::disable();

    uart_c0::port_traits::setup_pins();
    uart_c1::port_traits::setup_pins();
    uart_d0::port_traits::setup_pins();
    uart_e0::port_traits::setup_pins();
    uart_e1::port_traits::setup_pins();
    uart_f0::port_traits::setup_pins();
    uart_f1::port_traits::setup_pins();

    PORTC.INT0MASK = (1 << 2);
    PORTC.INTCTRL = PORT_INT0LVL_HI_gc;
}

uint8_t send(uint8_t port, const uint8_t *buf, uint8_t size) {
    switch (port) {
    case 0: return uart_c0::write_buf(buf, size);
    case 1: return uart_c1::write_buf(buf, size);
    case 2: return uart_d0::write_buf(buf, size);
    case 3: return uart_e0::write_buf(buf, size);
    case 4: return uart_e1::write_buf(buf, size);
    case 5: return uart_f0::write_buf(buf, size);
    case 6: return uart_f1::write_buf(buf, size);
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

ISR(PORTC_INT0_vect)
{
    crit_sec cs;

    bool v = uart_c0::rx::read();

    if (v) {
        uart_c0::tx::high();
        uart_c1::tx::high();
        uart_d0::tx::high();
        uart_e0::tx::high();
        uart_e1::tx::high();
        uart_f0::tx::high();
        uart_f1::tx::high();
    } else {
        uart_c0::tx::low();
        uart_c1::tx::low();
        uart_d0::tx::low();
        uart_e0::tx::low();
        uart_e1::tx::low();
        uart_f0::tx::low();
        uart_f1::tx::low();

        midi::rx_ready = 1;
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


ISR(USARTE1_RXC_vect)
{
    uart_e1::on_rxc_int();
}

ISR(USARTE1_DRE_vect)
{
    uart_e1::on_dre_int();
}


ISR(USARTF0_RXC_vect)
{
    uart_f0::on_rxc_int();
}

ISR(USARTF0_DRE_vect)
{
    uart_f0::on_dre_int();
}


ISR(USARTF1_RXC_vect)
{
    uart_f1::on_rxc_int();
}

ISR(USARTF1_DRE_vect)
{
    uart_f1::on_dre_int();
}
