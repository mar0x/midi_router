
#pragma once

#include <stdint.h>
#include <avr/io.h>

#include "port.h"

namespace artl {

namespace port {

    template<typename PORT>
    struct traits {
    };

    namespace mega {
        template<typename PORT>
        struct traits_base {
        };

        template<typename PORT>
        struct traits {
            using base_type = traits_base<PORT>;

            static volatile uint8_t& dir() { return base_type::dir(); }
            static void dirset(uint8_t mask) { dir() |= mask; }
            static void dirclr(uint8_t mask) { dir() &= ~mask; }
            static void dirtgl(uint8_t mask) { dir() ^= mask; }
            static uint8_t in() { return base_type::in(); }
            static volatile uint8_t& out() { return base_type::out(); }
            static void outset(uint8_t mask) { out() |= mask; }
            static void outclr(uint8_t mask) { out() &= ~mask; }
            static void outtgl(uint8_t mask) { out() ^= mask; }
        };
    }

    namespace xmega {
        template<typename PORT>
        struct traits_base {
        };

        template<typename PORT>
        struct traits {
            using base_type = traits_base<PORT>;

            static volatile uint8_t& dir() { return base_type::port().DIR; }
            static void dirset(uint8_t mask) { base_type::port().DIRSET = mask; }
            static void dirclr(uint8_t mask) { base_type::port().DIRCLR = mask; }
            static void dirtgl(uint8_t mask) { base_type::port().DIRTGL = mask; }
            static uint8_t in() { return base_type::port().IN; }
            static volatile uint8_t& out() { return base_type::port().OUT; }
            static void outset(uint8_t mask) { base_type::port().OUTSET = mask; }
            static void outclr(uint8_t mask) { base_type::port().OUTCLR = mask; }
            static void outtgl(uint8_t mask) { base_type::port().OUTTGL = mask; }
            static volatile uint8_t& intctrl() { return base_type::port().INTCTRL; }
            static volatile uint8_t& int0mask() { return base_type::port().INT0MASK; }
            static volatile uint8_t& int1mask() { return base_type::port().INT1MASK; }
            static volatile uint8_t& intflags() { return base_type::port().INTFLAGS; }

            inline __attribute__((always_inline))
            static volatile uint8_t* pinctrl() { return &base_type::port().PIN0CTRL; }
        };
    }

#if defined(PORTA)
#if defined(DDRA) && defined(PINA)

    namespace mega {
        template<>
        struct traits_base<A> {
            static volatile uint8_t& dir() { return DDRA; }
            static uint8_t in() { return PINA; }
            static volatile uint8_t& out() { return PORTA; }
        };
    }

    template<>
    struct traits<A> : public mega::traits<A> { };

#else

    namespace xmega {
        template<>
        struct traits_base<A> {
            static volatile PORT_t& port() { return PORTA; }
        };
    }

    template<>
    struct traits<A> : public xmega::traits<A> { };

#endif
#endif

#if defined(PORTB)
#if defined(DDRB) && defined(PINB)

    namespace mega {
        template<>
        struct traits_base<B> {
            static volatile uint8_t& dir() { return DDRB; }
            static uint8_t in() { return PINB; }
            static volatile uint8_t& out() { return PORTB; }
        };
    }

    template<>
    struct traits<B> : public mega::traits<B> { };

#else

    namespace xmega {
        template<>
        struct traits_base<B> {
            static volatile PORT_t& port() { return PORTB; }
        };
    }

    template<>
    struct traits<B> : public xmega::traits<B> { };

#endif
#endif

#if defined(PORTC)
#if defined(DDRC) && defined(PINC)

    namespace mega {
        template<>
        struct traits_base<C> {
            static volatile uint8_t& dir() { return DDRC; }
            static uint8_t in() { return PINC; }
            static volatile uint8_t& out() { return PORTC; }
        };
    }

    template<>
    struct traits<C> : public mega::traits<C> { };

#else

    namespace xmega {
        template<>
        struct traits_base<C> {
            static volatile PORT_t& port() { return PORTC; }
        };
    }

    template<>
    struct traits<C> : public xmega::traits<C> { };

#endif
#endif

#if defined(PORTD)
#if defined(DDRD) && defined(PIND)

    namespace mega {
        template<>
        struct traits_base<D> {
            static volatile uint8_t& dir() { return DDRD; }
            static uint8_t in() { return PIND; }
            static volatile uint8_t& out() { return PORTD; }
        };
    }

    template<>
    struct traits<D> : public mega::traits<D> { };

#else

    namespace xmega {
        template<>
        struct traits_base<D> {
            static volatile PORT_t& port() { return PORTD; }
        };
    }

    template<>
    struct traits<D> : public xmega::traits<D> { };

#endif
#endif

#if defined(PORTE)
#if defined(DDRE) && defined(PINE)

    namespace mega {
        template<>
        struct traits_base<E> {
            static volatile uint8_t& dir() { return DDRE; }
            static uint8_t in() { return PINE; }
            static volatile uint8_t& out() { return PORTE; }
        };
    }

    template<>
    struct traits<E> : public mega::traits<E> { };

#else

    namespace xmega {
        template<>
        struct traits_base<E> {
            static volatile PORT_t& port() { return PORTE; }
        };
    }

    template<>
    struct traits<E> : public xmega::traits<E> { };

#endif
#endif

#if defined(PORTF)
#if defined(DDRF) && defined(PINF)

    namespace mega {
        template<>
        struct traits_base<F> {
            static volatile uint8_t& dir() { return DDRF; }
            static uint8_t in() { return PINF; }
            static volatile uint8_t& out() { return PORTF; }
        };
    }

    template<>
    struct traits<F> : public mega::traits<F> { };

#else

    namespace xmega {
        template<>
        struct traits_base<F> {
            static volatile PORT_t& port() { return PORTF; }
        };
    }

    template<>
    struct traits<F> : public xmega::traits<F> { };

#endif
#endif

#if defined(PORTH)

    namespace xmega {
        template<>
        struct traits_base<H> {
            static volatile PORT_t& port() { return PORTH; }
        };
    }

    template<>
    struct traits<H> : public xmega::traits<H> { };

#endif

#if defined(PORTJ)

    namespace xmega {
        template<>
        struct traits_base<J> {
            static volatile PORT_t& port() { return PORTJ; }
        };
    }

    template<>
    struct traits<J> : public xmega::traits<J> { };

#endif

#if defined(PORTK)

    namespace xmega {
        template<>
        struct traits_base<K> {
            static volatile PORT_t& port() { return PORTK; }
        };
    }

    template<>
    struct traits<K> : public xmega::traits<K> { };

#endif

#if defined(PORTQ)

    namespace xmega {
        template<>
        struct traits_base<Q> {
            static volatile PORT_t& port() { return PORTQ; }
        };
    }

    template<>
    struct traits<Q> : public xmega::traits<Q> { };

#endif

} // namespace port

} // namespace artl

