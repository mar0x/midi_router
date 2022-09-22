#pragma once

#include "artl/digital_out.h"
#include "artl/digital_in.h"

#include "ring.h"
#include "crit_sec.h"

namespace port {

template<typename PORT>
struct traits { };

template<typename PORT, uint8_t RX_BIT, uint8_t TX_BIT>
struct pin_traits : public artl::port::traits<PORT> {
    using port_traits = artl::port::traits<PORT>;
    using rx = artl::digital_in<PORT, RX_BIT>;
    using tx = artl::digital_out<PORT, TX_BIT>;

    static void setup_pins() {
        setup_rx();
        setup_tx();
    }

    static void setup_rx() {
        port_traits::pinctrl()[rx::traits::bit_no] = 0;
        rx::input();
        rx::pulldown();
    }

    static void setup_tx() {
        port_traits::pinctrl()[tx::traits::bit_no] = 0;
        tx::output();
        tx::high();
    }
};

struct C0 { };

template<>
struct traits<C0> : public pin_traits<artl::port::C, 2, 3> {
    static USART_t& uart() { return USARTC0; }
    static void setup_pr() { PR.PRPC &= ~PR_USART0_bm; }

    enum {
        INT_LEVEL = PORT_INT0LVL_HI_gc,
        INT_FLAGS = PORT_INT0IF_bm,
    };
    static volatile uint8_t& intmask() { return port_traits::int0mask(); }
};

struct C1 { };

template<>
struct traits<C1> : public pin_traits<artl::port::C, 6, 7> {
    static USART_t& uart() { return USARTC1; }
    static void setup_pr() { PR.PRPC &= ~PR_USART1_bm; }

    enum {
        INT_LEVEL = PORT_INT1LVL_HI_gc,
        INT_FLAGS = PORT_INT1IF_bm,
    };
    static volatile uint8_t& intmask() { return port_traits::int1mask(); }
};

struct D0 { };

template<>
struct traits<D0> : public pin_traits<artl::port::D, 2, 3> {
    static USART_t& uart() { return USARTD0; }
    static void setup_pr() { PR.PRPD &= ~PR_USART0_bm; }

    enum {
        INT_LEVEL = PORT_INT0LVL_HI_gc,
        INT_FLAGS = PORT_INT0IF_bm,
    };
    static volatile uint8_t& intmask() { return port_traits::int0mask(); }
};

struct E0 { };

template<>
struct traits<E0> : public pin_traits<artl::port::E, 2, 3> {
    static USART_t& uart() { return USARTE0; }
    static void setup_pr() { PR.PRPE &= ~PR_USART0_bm; }

    enum {
        INT_LEVEL = PORT_INT0LVL_HI_gc,
        INT_FLAGS = PORT_INT0IF_bm,
    };
    static volatile uint8_t& intmask() { return port_traits::int0mask(); }
};


#if defined(USARTE1)

struct E1 { };

template<>
struct traits<E1> : public pin_traits<artl::port::E, 6, 7> {
    static USART_t& uart() { return USARTE1; }
    static void setup_pr() { PR.PRPE &= ~PR_USART1_bm; }

    enum {
        INT_LEVEL = PORT_INT1LVL_HI_gc,
        INT_FLAGS = PORT_INT1IF_bm,
    };
    static volatile uint8_t& intmask() { return port_traits::int1mask(); }
};

#endif

#if defined(USARTF0)

struct F0 { };

template<>
struct traits<F0> : public pin_traits<artl::port::F, 2, 3> {
    static USART_t& uart() { return USARTF0; }
    static void setup_pr() { PR.PRPF &= ~PR_USART0_bm; }

    enum {
        INT_LEVEL = PORT_INT0LVL_HI_gc,
        INT_FLAGS = PORT_INT0IF_bm,
    };
    static volatile uint8_t& intmask() { return port_traits::int0mask(); }
};

#endif

#if defined(USARTF1)

struct F1 { };

template<>
struct traits<F1> : public pin_traits<artl::port::F, 6, 7> {
    static USART_t& uart() { return USARTF1; }
    static void setup_pr() { PR.PRPF &= ~PR_USART1_bm; }

    enum {
        INT_LEVEL = PORT_INT1LVL_HI_gc,
        INT_FLAGS = PORT_INT1IF_bm,
    };
    static volatile uint8_t& intmask() { return port_traits::int1mask(); }
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

template<uint16_t MAX, typename T, typename PORT>
struct rx_ring_traits {
    using port_t = PORT;
    using rx_ring_t = ring<MAX, T>;

    static bool ring_read(uint8_t &b) {
        if (!rx_ring.empty()) {
            b = rx_ring.pop_front();
            return true;
        }

        return false;
    }

    static bool on_rx(uint8_t d) {
        if (!rx_ring.full()) {
            rx_ring.push_back(d);
            return true;
        }

        return false;
    }

private:
    static rx_ring_t rx_ring;
};

struct tx_dummy_traits {
    static inline void tx_ready(uint8_t s) { }
};

template<
    typename PORT, unsigned long RATE,
    typename RX_TRAITS = rx_ring_traits<64, uint8_t, PORT>,
    typename TX_TRAITS = tx_dummy_traits,
    typename PORT_TRAITS = port::traits<PORT>,
    typename BAUD_TRAITS = baud_traits<RATE, F_CPU>,
    typename TX_RING = ring<64, uint8_t> >
struct uart_t : public RX_TRAITS {
    using rx_traits = RX_TRAITS;
    using tx_traits = TX_TRAITS;
    using port_traits = PORT_TRAITS;
    using baud_traits = BAUD_TRAITS;

    using rx = typename port_traits::rx;
    using tx = typename port_traits::tx;

    using tx_ring_t = TX_RING;

    enum {
        tx_mask = (1 << tx_traits::id),
    };

    static USART_t& uart() { return port_traits::uart(); }

    static bool dre() { return (uart().STATUS & USART_DREIF_bm) != 0; }
    static bool ferr() { return (uart().STATUS & USART_FERR_bm) != 0; }
    static bool bufovf() { return (uart().STATUS & USART_BUFOVF_bm) != 0; }

    static bool tx_enabled() { return (uart().CTRLB & USART_TXEN_bm) != 0; }

    static void rxc_int_off() { uart().CTRLA &= ~USART_RXCINTLVL_gm; }
    static void rxc_int_lo() { uart().CTRLA |= USART_RXCINTLVL_LO_gc; }
    static void rxc_int_med() { uart().CTRLA |= USART_RXCINTLVL_MED_gc; }
    static void rxc_int_hi() { uart().CTRLA |= USART_RXCINTLVL_HI_gc; }

    static void txc_int_off() { uart().CTRLA &= ~USART_TXCINTLVL_gm; }
    static void txc_int_lo() { uart().CTRLA |= USART_TXCINTLVL_LO_gc; }
    static void txc_int_med() { uart().CTRLA |= USART_TXCINTLVL_MED_gc; }
    static void txc_int_hi() { uart().CTRLA |= USART_TXCINTLVL_HI_gc; }

    static void dre_int_off() { uart().CTRLA &= ~USART_DREINTLVL_gm; }
    static void dre_int_lo() { uart().CTRLA |= USART_DREINTLVL_LO_gc; }
    static void dre_int_med() { uart().CTRLA |= USART_DREINTLVL_MED_gc; }
    static void dre_int_hi() { uart().CTRLA |= USART_DREINTLVL_HI_gc; }

    static uint8_t data() { return uart().DATA; }
    static void data(uint8_t d) { uart().DATA = d; }

    static void setup() {
        port_traits::setup_pins();
        port_traits::setup_pr();

        port_traits::intmask() |= rx::traits::bit_mask;

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

    static void disable_tx() {
        dre_int_off();
        uart().CTRLB &= ~USART_TXEN_bm;
    }

    static void enable_tx() {
        uart().CTRLB |= USART_TXEN_bm;
    }

    static uint8_t write_buf(const void *d, uint8_t s) {
        if (s == 1 && tx_ring_empty() && dre()) {
            data(* (const uint8_t *) d);

            return 1;
        }

        uint8_t res;

        crit_sec cs;

        if (tx_ring_avail() > s) {
            const uint8_t *c = (const uint8_t *) d;

            res = s;

            if (tx_ring_empty() && dre()) {
                data(*c);

                ++c;
                --s;
            }

            while (s != 0) {
                tx_ring_push(*c);

                ++c;
                --s;
            }

            while (dre() && !tx_ring_empty()) { data(tx_ring_pop()); }

            if (!tx_ring_empty()) { dre_int_hi(); }

            want_write = 0;
        } else {
            res = 0;
            want_write = s;
        }

        return res;
    }

    static uint8_t write_byte(uint8_t b) {
        if (tx_ring_empty() && dre()) {
            data(b);

            return 1;
        }

        if (tx_ring_avail() > 1) {
            tx_ring_push(b);

            while (dre() && !tx_ring_empty()) { data(tx_ring_pop()); }

            if (!tx_ring_empty()) { dre_int_hi(); }

            want_write = 0;

            return 1;
        } else {
            want_write = 1;

            return 0;
        }
    }

    static void on_rxc_int() {
        rx_traits::on_rx(data());
    }

    static void on_dre_int() {
        if (!tx_ring_empty()) {
            data(tx_ring_pop());

            if (want_write && tx_ring_avail() > want_write) {
                tx_traits::tx_ready(tx_ring_avail() - 1);
            }
        } else {
            dre_int_off();
        }
    }

    static inline bool tx_ring_empty() { return tx_ring.empty(); }
    static inline uint8_t tx_ring_pop() { return tx_ring.pop_front(); }
    static inline void tx_ring_push(uint8_t b) { tx_ring.push_back(b); }
    static inline uint8_t tx_ring_avail() { return tx_ring.capacity - tx_ring.size(); }

    static inline void enable_pin_ch() {
        port_traits::intflags() = port_traits::INT_FLAGS;
        port_traits::intctrl() |= port_traits::INT_LEVEL;
    }
    static inline void disable_pin_ch() {
        port_traits::intctrl() &= ~port_traits::INT_LEVEL;
        port_traits::intflags() = port_traits::INT_FLAGS;
    }
private:
    static tx_ring_t tx_ring;
    static uint8_t want_write;
};

template<typename...>
struct uart_list;

template<typename T>
struct uart_list<T> {
    enum {
        tx_mask = T::tx_mask,
    };

    static void setup() { T::setup(); }
    static void enable_tx() { T::enable_tx(); }
    static void disable_tx() { T::disable_tx(); }
    static void rxc_int_off() { T::rxc_int_off(); }
    static void rxc_int_hi() { T::rxc_int_hi(); }
    static void txc_int_off() { T::txc_int_off(); }
    static void txc_int_hi() { T::txc_int_hi(); }

    static void setup_tx_pin() { T::port_traits::setup_tx(); }
    static void tx_high() { T::tx::high(); }
    static void tx_low() { T::tx::low(); }

    static bool tx_ring_empty() { return T::tx_ring_empty(); }
    static uint8_t tx_ring_non_empty_mask() {
        return T::tx_ring_empty() ? 0 : (1 << T::tx_traits::id);
    }

    static void data(uint8_t d) { T::data(d); }
    static void write_byte(uint8_t b) { T::write_byte(b); }
    static uint8_t write_buf(uint8_t port, const uint8_t *buf, uint8_t size) {
        if (port == T::tx_traits::id) {
            return T::write_buf(buf, size);
        }
        return 0;
    }

    static void enable_pin_ch() { T::enable_pin_ch(); }
    static void enable_pin_ch(uint8_t id) {
        if (id == T::rx_traits::id) {
            T::enable_pin_ch();
        }
    }
    static void disable_pin_ch() { T::disable_pin_ch(); }
    static void disable_pin_ch(uint8_t id) {
        if (id == T::rx_traits::id) {
            T::disable_pin_ch();
        }
    }
};

template<typename T, typename... T2>
struct uart_list<T, T2...> {
    using L = uart_list<T2...>;

    enum {
        tx_mask = T::tx_mask | L::tx_mask,
    };

    static void setup() { T::setup(); L::setup(); }
    static void enable_tx() { T::enable_tx(); L::enable_tx(); }
    static void disable_tx() { T::disable_tx(); L::disable_tx(); }
    static void rxc_int_off() { T::rxc_int_off(); L::rxc_int_off(); }
    static void txc_int_off() { T::txc_int_off(); L::txc_int_off(); }
    static void rxc_int_hi() { T::rxc_int_hi(); L::rxc_int_hi(); }
    static void txc_int_hi() { T::txc_int_hi(); L::txc_int_hi(); }

    static void setup_tx_pin() { T::port_traits::setup_tx(); L::setup_tx_pin(); }
    static void tx_high() { T::tx::high(); L::tx_high(); }
    static void tx_low() { T::tx::low(); L::tx_low(); }

    static bool tx_ring_empty() { return T::tx_ring_empty(); }
    static uint8_t tx_ring_non_empty_mask() {
        return (T::tx_ring_empty() ? 0 : (1 << T::tx_traits::id))
            | L::tx_ring_non_empty_mask();
    }

    static void data(uint8_t d) { T::data(d); L::data(d); }
    static void write_byte(uint8_t b) { T::write_byte(b); L::write_byte(b); }
    static uint8_t write_buf(uint8_t port, const uint8_t *buf, uint8_t size) {
        if (port == T::tx_traits::id) {
            return T::write_buf(buf, size);
        } else {
            return L::write_buf(port, buf, size);
        }
    }

    static void enable_pin_ch() { T::enable_pin_ch(); L::enable_pin_ch(); }
    static void enable_pin_ch(uint8_t id) {
        if (id == T::rx_traits::id) {
            T::enable_pin_ch();
        } else {
            L::enable_pin_ch(id);
        }
    }
    static void disable_pin_ch() { T::disable_pin_ch(); L::disable_pin_ch(); }
    static void disable_pin_ch(uint8_t id) {
        if (id == T::rx_traits::id) {
            T::disable_pin_ch();
        } else {
            L::disable_pin_ch(id);
        }
    }

    template<typename CB>
    static void tx_enum(CB cb) {
        CB::enum_action(T::tx_traits::id);
    }
};
