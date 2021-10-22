#pragma once

#include "artl/digital_out.h"
#include "artl/digital_in.h"

#include "ring.h"
#include "crit_sec.h"

namespace port {

template<typename PORT>
struct traits { };

struct C0 { };

template<>
struct traits<C0> {
    using rx = artl::digital_in<artl::port::C, 2>;
    using tx = artl::digital_out<artl::port::C, 3>;

    static USART_t& uart() { return USARTC0; }

    static void setup_pins() {
        PORTC.PIN3CTRL = 0;
        tx::output();
        tx::high();

        PORTC.PIN2CTRL = 0;
        rx::input();
        rx::pulldown();

        PR.PRPC &= ~PR_USART0_bm;
    }
};

struct C1 { };

template<>
struct traits<C1> {
    using rx = artl::digital_in<artl::port::C, 6>;
    using tx = artl::digital_out<artl::port::C, 7>;

    static USART_t& uart() { return USARTC1; }

    static void setup_pins() {
        PORTC.PIN7CTRL = 0;
        tx::output();
        tx::high();

        PORTC.PIN6CTRL = 0;
        rx::input();
        rx::pulldown();

        PR.PRPC &= ~PR_USART1_bm;
    }
};

struct D0 { };

template<>
struct traits<D0> {
    using rx = artl::digital_in<artl::port::D, 2>;
    using tx = artl::digital_out<artl::port::D, 3>;

    static USART_t& uart() { return USARTD0; }

    static void setup_pins() {
        PORTD.PIN3CTRL = 0;
        tx::output();
        tx::high();

        PORTD.PIN2CTRL = 0;
        rx::input();
        rx::pulldown();

        PR.PRPD &= ~PR_USART0_bm;
    }
};

struct E0 { };

template<>
struct traits<E0> {
    using rx = artl::digital_in<artl::port::E, 2>;
    using tx = artl::digital_out<artl::port::E, 3>;

    static USART_t& uart() { return USARTE0; }

    static void setup_pins() {
        PORTE.PIN3CTRL = 0;
        tx::output();
        tx::high();

        PORTE.PIN2CTRL = 0;
        rx::input();
        rx::pulldown();

        PR.PRPE &= ~PR_USART0_bm;
    }
};


#if defined(USARTE1)

struct E1 { };

template<>
struct traits<E1> {
    using rx = artl::digital_in<artl::port::E, 6>;
    using tx = artl::digital_out<artl::port::E, 7>;

    static USART_t& uart() { return USARTE1; }

    static void setup_pins() {
        PORTE.PIN7CTRL = 0;
        tx::output();
        tx::high();

        PORTE.PIN6CTRL = 0;
        rx::input();
        rx::pulldown();

        PR.PRPE &= ~PR_USART1_bm;
    }
};

#endif

#if defined(USARTF0)

struct F0 { };

template<>
struct traits<F0> {
    using rx = artl::digital_in<artl::port::F, 2>;
    using tx = artl::digital_out<artl::port::F, 3>;

    static USART_t& uart() { return USARTF0; }

    static void setup_pins() {
        PORTF.PIN3CTRL = 0;
        tx::output();
        tx::high();

        PORTF.PIN2CTRL = 0;
        rx::input();
        rx::pulldown();

        PR.PRPF &= ~PR_USART0_bm;
    }
};

#endif

#if defined(USARTF1)

struct F1 { };

template<>
struct traits<F1> {
    using rx = artl::digital_in<artl::port::F, 6>;
    using tx = artl::digital_out<artl::port::F, 7>;

    static USART_t& uart() { return USARTF1; }

    static void setup_pins() {
        PORTF.PIN7CTRL = 0;
        tx::output();
        tx::high();

        PORTF.PIN6CTRL = 0;
        rx::input();
        rx::pulldown();

        PR.PRPF &= ~PR_USART1_bm;
    }
};

#endif

}


template<unsigned long RATE, unsigned long f_cpu>
struct baud_traits { };

template<>
struct baud_traits<500000, 32000000> {
// F_CPU = 32 MHz, BAUD = 500000, BSCALE = 3 (0x3), BSEL = 0 (0x0)
    enum { ctrla = 0x3, ctrlb = 0x0, };
};

template<>
struct baud_traits<500000, 24000000> {
// F_CPU = 24 MHz, BAUD = 500000, BSCALE = 2 (0x2), BSEL = 0 (0x0)
    enum { ctrla = 0x2, ctrlb = 0x0, };
};

template<>
struct baud_traits<500000, 16000000> {
// F_CPU = 16 MHz, BAUD = 500000, BSCALE = 1 (0x1), BSEL = 0 (0x0)
    enum { ctrla = 0x1, ctrlb = 0x0, };
};

template<>
struct baud_traits<115200, 32000000> {
// F_CPU = 32 MHz, BAUD = 115200, BSCALE = 2094 (0x82E), BSEL = -7 (0x9)
    enum { ctrla = 0x2E, ctrlb = 0x98, };
};

template<>
struct baud_traits<115200, 24000000> {
// F_CPU = 24 MHz, BAUD = 115200, BSCALE = 1539 (0x603), BSEL = -7 (0x9)
    enum { ctrla = 0x03, ctrlb = 0x96, };
};

template<>
struct baud_traits<115200, 16000000> {
// F_CPU = 16 MHz, BAUD = 115200, BSCALE = 983 (0x3D7), BSEL = -7 (0x9)
    enum { ctrla = 0xD7, ctrlb = 0x93, };
};


template<>
struct baud_traits<31250, 32000000> {
// F_CPU = 32 MHz, BAUD = 31250, BSCALE = 63 (0x3F), BSEL = 0 (0x0)
    enum { ctrla = 0x3F, ctrlb = 0x00, };
};

template<>
struct baud_traits<31250, 24000000> {
// F_CPU = 24 MHz, BAUD = 31250, BSCALE = 47 (0x2F), BSEL = 0 (0x0)
    enum { ctrla = 0x2F, ctrlb = 0x00, };
};

template<>
struct baud_traits<31250, 16000000> {
// F_CPU = 16 MHz, BAUD = 31250, BSCALE = 31 (0x1F), BSEL = 0 (0x0)
    enum { ctrla = 0x1F, ctrlb = 0x00, };
};

template<
    typename PORT, unsigned long RATE,
    typename PORT_TRAITS = port::traits<PORT>,
    typename BAUD_TRAITS = baud_traits<RATE, F_CPU>,
    typename RX_RING = ring<64, uint8_t>,
    typename TX_RING = ring<64, uint8_t> >
struct uart_t {
    using port_traits = PORT_TRAITS;
    using baud_traits = BAUD_TRAITS;

    using rx = typename port_traits::rx;
    using tx = typename port_traits::tx;

    using rx_ring_t = RX_RING;
    using tx_ring_t = TX_RING;

    static USART_t& uart() { return port_traits::uart(); }

    static bool dre() { return (uart().STATUS & USART_DREIF_bm) != 0; }
    static bool ferr() { return (uart().STATUS & USART_FERR_bm) != 0; }
    static bool bufovf() { return (uart().STATUS & USART_BUFOVF_bm) != 0; }

    static void rxc_int_off() { uart().CTRLA &= ~USART_RXCINTLVL_gm; }
    static void rxc_int_lo() { uart().CTRLA |= USART_RXCINTLVL_LO_gc; }
    static void rxc_int_med() { uart().CTRLA |= USART_RXCINTLVL_MED_gc; }
    static void rxc_int_hi() { uart().CTRLA |= USART_RXCINTLVL_HI_gc; }

    static void dre_int_off() { uart().CTRLA &= ~USART_DREINTLVL_gm; }
    static void dre_int_lo() { uart().CTRLA |= USART_DREINTLVL_LO_gc; }
    static void dre_int_med() { uart().CTRLA |= USART_DREINTLVL_MED_gc; }
    static void dre_int_hi() { uart().CTRLA |= USART_DREINTLVL_HI_gc; }

    static uint8_t data() { return uart().DATA; }
    static void data(uint8_t d) { uart().DATA = d; }

    static void setup() {
        port_traits::setup_pins();

        uart().CTRLA = USART_RXCINTLVL_OFF_gc | USART_TXCINTLVL_OFF_gc | USART_DREINTLVL_OFF_gc;
        uart().CTRLB = USART_RXEN_bm | USART_TXEN_bm;
        uart().CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;

        uart().BAUDCTRLA = baud_traits::ctrla;
        uart().BAUDCTRLB = baud_traits::ctrlb;
    }

    static void disable() {
        uart().CTRLA = USART_RXCINTLVL_OFF_gc | USART_TXCINTLVL_OFF_gc | USART_DREINTLVL_OFF_gc;
        uart().CTRLB = 0;
    }

    static bool bread(uint8_t &b) {
        if (!rx_ring.empty()) {
            b = rx_ring.pop_front();
            return true;
        }

        return false;
    }

    static uint8_t bwrite(uint8_t b) {
        if (tx_ring.full()) {
            return 1;
        } else {
            tx_ring.push_back(b);
        }

        return 0;
    }

    static uint8_t bwrite(const void *d, uint8_t s) {
        const uint8_t *c = (const uint8_t *) d;

        while (s != 0) {
            if (tx_ring.full()) {
                break;
            }

            tx_ring.push_back(*c);

            ++c;
            --s;
        }

        return s;
    }

    static void bflush() {
        while (dre() && !tx_ring.empty()) { data(tx_ring.pop_front()); }

        if (!tx_ring.empty()) { dre_int_hi(); }
    }

    static uint8_t write_buf(const void *d, uint8_t s) {
        if (s == 1 && tx_ring.empty() && dre()) {
            data(* (const uint8_t *) d);

            return 1;
        }

        crit_sec cs;

        uint8_t res = s - bwrite(d, s);

        bflush();

        return res;
    }

    static void on_rxc_int() {
        rx_ring_t& rx_r = rx_ring;

        if (!rx_r.full()) {
            rx_r.push_back(data());
        }
    }

    static void on_dre_int() {
        if (!tx_ring.empty()) {
            data(tx_ring.pop_front());
            return;
        }

        dre_int_off();
    }

private:
    static rx_ring_t rx_ring;
    static tx_ring_t tx_ring;
};
