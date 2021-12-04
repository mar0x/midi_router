
#include <uart.h>
#include <crit_sec.h>

namespace {

enum {
    MAX_PORT = 7
};

volatile bool port_ready[MAX_PORT];

using uart_c0 = uart_t<port::C0, 31250>;
template<> uart_c0::rx_ring_t uart_c0::rx_ring = {};
template<> uart_c0::tx_ring_t uart_c0::tx_ring = {};

using uart_c1 = uart_t<port::C1, 31250>;
template<> uart_c1::rx_ring_t uart_c1::rx_ring = {};
template<> uart_c1::tx_ring_t uart_c1::tx_ring = {};

using uart_d0 = uart_t<port::D0, 31250>;
template<> uart_d0::rx_ring_t uart_d0::rx_ring = {};
template<> uart_d0::tx_ring_t uart_d0::tx_ring = {};

using uart_e0 = uart_t<port::E0, 31250>;
template<> uart_e0::rx_ring_t uart_e0::rx_ring = {};
template<> uart_e0::tx_ring_t uart_e0::tx_ring = {};

using uart_e1 = uart_t<port::E1, 31250>;
template<> uart_e1::rx_ring_t uart_e1::rx_ring = {};
template<> uart_e1::tx_ring_t uart_e1::tx_ring = {};

using uart_f0 = uart_t<port::F0, 31250>;
template<> uart_f0::rx_ring_t uart_f0::rx_ring = {};
template<> uart_f0::tx_ring_t uart_f0::tx_ring = {};

using uart_f1 = uart_t<port::F1, 31250>;
template<> uart_f1::rx_ring_t uart_f1::rx_ring = {};
template<> uart_f1::tx_ring_t uart_f1::tx_ring = {};

inline
bool port_read(uint8_t port, uint8_t &data) {
    switch (port) {
    case 0: return uart_c0::bread(data);
    case 1: return uart_c1::bread(data);
    case 2: return uart_d0::bread(data);
    case 3: return uart_e0::bread(data);
    case 4: return uart_e1::bread(data);
    case 5: return uart_f0::bread(data);
    case 6: return uart_f1::bread(data);
    }

    return false;
}

}

namespace midi {

volatile uint8_t rx_ready;

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

bool recv(uint8_t &port, uint8_t &data) {
    uint8_t ready = rx_ready;

    for (uint8_t i = 0; i < MAX_PORT; ++i) {
        uint8_t n = (i + ready) % MAX_PORT;

        if (port_ready[n]) {
            crit_sec cs;

            if (port_read(n, data)) {
                port = n;
                return true;
            }

            port_ready[n] = false;
        }
    }
    return false;
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

    port_ready[0] = true;
    midi::rx_ready = 0;
}

ISR(USARTC0_DRE_vect)
{
    uart_c0::on_dre_int();
}


ISR(USARTC1_RXC_vect)
{
    uart_c1::on_rxc_int();

    port_ready[1] = true;
    midi::rx_ready = 1;
}

ISR(USARTC1_DRE_vect)
{
    uart_c1::on_dre_int();
}


ISR(USARTD0_RXC_vect)
{
    uart_d0::on_rxc_int();

    port_ready[2] = true;
    midi::rx_ready = 2;
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

    port_ready[3] = true;
    midi::rx_ready = 3;
}

ISR(USARTE0_DRE_vect)
{
    uart_e0::on_dre_int();
}


ISR(USARTE1_RXC_vect)
{
    uart_e1::on_rxc_int();

    port_ready[4] = true;
    midi::rx_ready = 4;
}

ISR(USARTE1_DRE_vect)
{
    uart_e1::on_dre_int();
}


ISR(USARTF0_RXC_vect)
{
    uart_f0::on_rxc_int();

    port_ready[5] = true;
    midi::rx_ready = 5;
}

ISR(USARTF0_DRE_vect)
{
    uart_f0::on_dre_int();
}


ISR(USARTF1_RXC_vect)
{
    uart_f1::on_rxc_int();

    port_ready[6] = true;
    midi::rx_ready = 6;
}

ISR(USARTF1_DRE_vect)
{
    uart_f1::on_dre_int();
}
