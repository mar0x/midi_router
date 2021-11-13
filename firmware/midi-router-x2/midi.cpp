
#include <uart.h>
#include <crit_sec.h>

namespace {

enum {
    MAX_PORT = 2
};

volatile bool port_ready[MAX_PORT];

using uart_c0 = uart_t<port::C0, 31250>;
template<> uart_c0::rx_ring_t uart_c0::rx_ring = {};
template<> uart_c0::tx_ring_t uart_c0::tx_ring = {};

using uart_c1 = uart_t<port::C1, 31250>;
template<> uart_c1::rx_ring_t uart_c1::rx_ring = {};
template<> uart_c1::tx_ring_t uart_c1::tx_ring = {};

inline
bool port_read(uint8_t port, uint8_t &data) {
    switch (port) {
    case 0: return uart_c0::bread(data);
    case 1: return uart_c1::bread(data);
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

    uart_c0::rxc_int_hi();
    uart_c1::rxc_int_hi();
}

void splitter() {
    uart_c0::disable();
    uart_c1::disable();

    uart_c0::port_traits::setup_pins();
    uart_c1::port_traits::setup_pins();

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


ISR(PORTC_INT0_vect)
{
    crit_sec cs;

    bool v = uart_c0::rx::read();

    if (v) {
        uart_c0::tx::high();
        uart_c1::tx::high();
    } else {
        uart_c0::tx::low();
        uart_c1::tx::low();
    }

    midi::rx_ready = 1;
}
