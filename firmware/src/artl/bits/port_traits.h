
#pragma once

#include <stdint.h>
#include <avr/io.h>

#include "port.h"

namespace artl {

namespace port {

    template<typename PORT>
    struct traits {
    };

#if defined(PORTA)
#if defined(DDRA) && defined(PINA)

    template<>
    struct traits<A> {
        static volatile uint8_t& dir() { return DDRA; }
        static void dirset(uint8_t mask) { dir() |= mask; }
        static void dirclr(uint8_t mask) { dir() &= ~mask; }
        static void dirtgl(uint8_t mask) { dir() ^= mask; }
        static uint8_t in() { return PINA; }
        static volatile uint8_t& out() { return PORTA; }
        static void outset(uint8_t mask) { out() |= mask; }
        static void outclr(uint8_t mask) { out() &= ~mask; }
        static void outtgl(uint8_t mask) { out() ^= mask; }
    };

#else

    template<>
    struct traits<A> {
        static volatile uint8_t& dir() { return PORTA.DIR; }
        static void dirset(uint8_t mask) { PORTA.DIRSET = mask; }
        static void dirclr(uint8_t mask) { PORTA.DIRCLR = mask; }
        static void dirtgl(uint8_t mask) { PORTA.DIRTGL = mask; }
        static uint8_t in() { return PORTA.IN; }
        static volatile uint8_t& out() { return PORTA.OUT; }
        static void outset(uint8_t mask) { PORTA.OUTSET = mask; }
        static void outclr(uint8_t mask) { PORTA.OUTCLR = mask; }
        static void outtgl(uint8_t mask) { PORTA.OUTTGL = mask; }
    };

#endif
#endif

#if defined(PORTB)
#if defined(DDRB) && defined(PINB)

    template<>
    struct traits<B> {
        static volatile uint8_t& dir() { return DDRB; }
        static void dirset(uint8_t mask) { dir() |= mask; }
        static void dirclr(uint8_t mask) { dir() &= ~mask; }
        static void dirtgl(uint8_t mask) { dir() ^= mask; }
        static uint8_t in() { return PINB; }
        static volatile uint8_t& out() { return PORTB; }
        static void outset(uint8_t mask) { out() |= mask; }
        static void outclr(uint8_t mask) { out() &= ~mask; }
        static void outtgl(uint8_t mask) { out() ^= mask; }
    };

#else

    template<>
    struct traits<B> {
        static volatile uint8_t& dir() { return PORTB.DIR; }
        static void dirset(uint8_t mask) { PORTB.DIRSET = mask; }
        static void dirclr(uint8_t mask) { PORTB.DIRCLR = mask; }
        static void dirtgl(uint8_t mask) { PORTB.DIRTGL = mask; }
        static uint8_t in() { return PORTB.IN; }
        static volatile uint8_t& out() { return PORTB.OUT; }
        static void outset(uint8_t mask) { PORTB.OUTSET = mask; }
        static void outclr(uint8_t mask) { PORTB.OUTCLR = mask; }
        static void outtgl(uint8_t mask) { PORTB.OUTTGL = mask; }
    };

#endif
#endif

#if defined(PORTC)
#if defined(DDRC) && defined(PINC)

    template<>
    struct traits<C> {
        static volatile uint8_t& dir() { return DDRC; }
        static void dirset(uint8_t mask) { dir() |= mask; }
        static void dirclr(uint8_t mask) { dir() &= ~mask; }
        static void dirtgl(uint8_t mask) { dir() ^= mask; }
        static uint8_t in() { return PINC; }
        static volatile uint8_t& out() { return PORTC; }
        static void outset(uint8_t mask) { out() |= mask; }
        static void outclr(uint8_t mask) { out() &= ~mask; }
        static void outtgl(uint8_t mask) { out() ^= mask; }
    };

#else

    template<>
    struct traits<C> {
        static volatile uint8_t& dir() { return PORTC.DIR; }
        static void dirset(uint8_t mask) { PORTC.DIRSET = mask; }
        static void dirclr(uint8_t mask) { PORTC.DIRCLR = mask; }
        static void dirtgl(uint8_t mask) { PORTC.DIRTGL = mask; }
        static uint8_t in() { return PORTC.IN; }
        static volatile uint8_t& out() { return PORTC.OUT; }
        static void outset(uint8_t mask) { PORTC.OUTSET = mask; }
        static void outclr(uint8_t mask) { PORTC.OUTCLR = mask; }
        static void outtgl(uint8_t mask) { PORTC.OUTTGL = mask; }
    };

#endif
#endif

#if defined(PORTD)
#if defined(DDRD) && defined(PIND)

    template<>
    struct traits<D> {
        static volatile uint8_t& dir() { return DDRD; }
        static void dirset(uint8_t mask) { dir() |= mask; }
        static void dirclr(uint8_t mask) { dir() &= ~mask; }
        static void dirtgl(uint8_t mask) { dir() ^= mask; }
        static uint8_t in() { return PIND; }
        static volatile uint8_t& out() { return PORTD; }
        static void outset(uint8_t mask) { out() |= mask; }
        static void outclr(uint8_t mask) { out() &= ~mask; }
        static void outtgl(uint8_t mask) { out() ^= mask; }
    };

#else

    template<>
    struct traits<D> {
        static volatile uint8_t& dir() { return PORTD.DIR; }
        static void dirset(uint8_t mask) { PORTD.DIRSET = mask; }
        static void dirclr(uint8_t mask) { PORTD.DIRCLR = mask; }
        static void dirtgl(uint8_t mask) { PORTD.DIRTGL = mask; }
        static uint8_t in() { return PORTD.IN; }
        static volatile uint8_t& out() { return PORTD.OUT; }
        static void outset(uint8_t mask) { PORTD.OUTSET = mask; }
        static void outclr(uint8_t mask) { PORTD.OUTCLR = mask; }
        static void outtgl(uint8_t mask) { PORTD.OUTTGL = mask; }
    };

#endif
#endif

#if defined(PORTE)
#if defined(DDRE) && defined(PINE)

    template<>
    struct traits<E> {
        static volatile uint8_t& dir() { return DDRE; }
        static void dirset(uint8_t mask) { dir() |= mask; }
        static void dirclr(uint8_t mask) { dir() &= ~mask; }
        static void dirtgl(uint8_t mask) { dir() ^= mask; }
        static uint8_t in() { return PINE; }
        static volatile uint8_t& out() { return PORTE; }
        static void outset(uint8_t mask) { out() |= mask; }
        static void outclr(uint8_t mask) { out() &= ~mask; }
        static void outtgl(uint8_t mask) { out() ^= mask; }
    };

#else

    template<>
    struct traits<E> {
        static volatile uint8_t& dir() { return PORTE.DIR; }
        static void dirset(uint8_t mask) { PORTE.DIRSET = mask; }
        static void dirclr(uint8_t mask) { PORTE.DIRCLR = mask; }
        static void dirtgl(uint8_t mask) { PORTE.DIRTGL = mask; }
        static uint8_t in() { return PORTE.IN; }
        static volatile uint8_t& out() { return PORTE.OUT; }
        static void outset(uint8_t mask) { PORTE.OUTSET = mask; }
        static void outclr(uint8_t mask) { PORTE.OUTCLR = mask; }
        static void outtgl(uint8_t mask) { PORTE.OUTTGL = mask; }
    };

#endif
#endif

#if defined(PORTF)
#if defined(DDRF) && defined(PINF)

    template<>
    struct traits<F> {
        static volatile uint8_t& dir() { return DDRF; }
        static void dirset(uint8_t mask) { dir() |= mask; }
        static void dirclr(uint8_t mask) { dir() &= ~mask; }
        static void dirtgl(uint8_t mask) { dir() ^= mask; }
        static uint8_t in() { return PINF; }
        static volatile uint8_t& out() { return PORTF; }
        static void outset(uint8_t mask) { out() |= mask; }
        static void outclr(uint8_t mask) { out() &= ~mask; }
        static void outtgl(uint8_t mask) { out() ^= mask; }
    };

#else

    template<>
    struct traits<F> {
        static volatile uint8_t& dir() { return PORTF.DIR; }
        static void dirset(uint8_t mask) { PORTF.DIRSET = mask; }
        static void dirclr(uint8_t mask) { PORTF.DIRCLR = mask; }
        static void dirtgl(uint8_t mask) { PORTF.DIRTGL = mask; }
        static uint8_t in() { return PORTF.IN; }
        static volatile uint8_t& out() { return PORTF.OUT; }
        static void outset(uint8_t mask) { PORTF.OUTSET = mask; }
        static void outclr(uint8_t mask) { PORTF.OUTCLR = mask; }
        static void outtgl(uint8_t mask) { PORTF.OUTTGL = mask; }
    };

#endif
#endif

#if defined(PORTH)

    template<>
    struct traits<H> {
        static volatile uint8_t& dir() { return PORTH.DIR; }
        static void dirset(uint8_t mask) { PORTH.DIRSET = mask; }
        static void dirclr(uint8_t mask) { PORTH.DIRCLR = mask; }
        static void dirtgl(uint8_t mask) { PORTH.DIRTGL = mask; }
        static uint8_t in() { return PORTH.IN; }
        static volatile uint8_t& out() { return PORTH.OUT; }
        static void outset(uint8_t mask) { PORTH.OUTSET = mask; }
        static void outclr(uint8_t mask) { PORTH.OUTCLR = mask; }
        static void outtgl(uint8_t mask) { PORTH.OUTTGL = mask; }
    };

#endif

#if defined(PORTJ)

    template<>
    struct traits<J> {
        static volatile uint8_t& dir() { return PORTJ.DIR; }
        static void dirset(uint8_t mask) { PORTJ.DIRSET = mask; }
        static void dirclr(uint8_t mask) { PORTJ.DIRCLR = mask; }
        static void dirtgl(uint8_t mask) { PORTJ.DIRTGL = mask; }
        static uint8_t in() { return PORTJ.IN; }
        static volatile uint8_t& out() { return PORTJ.OUT; }
        static void outset(uint8_t mask) { PORTJ.OUTSET = mask; }
        static void outclr(uint8_t mask) { PORTJ.OUTCLR = mask; }
        static void outtgl(uint8_t mask) { PORTJ.OUTTGL = mask; }
    };

#endif

#if defined(PORTK)

    template<>
    struct traits<K> {
        static volatile uint8_t& dir() { return PORTK.DIR; }
        static void dirset(uint8_t mask) { PORTK.DIRSET = mask; }
        static void dirclr(uint8_t mask) { PORTK.DIRCLR = mask; }
        static void dirtgl(uint8_t mask) { PORTK.DIRTGL = mask; }
        static uint8_t in() { return PORTK.IN; }
        static volatile uint8_t& out() { return PORTK.OUT; }
        static void outset(uint8_t mask) { PORTK.OUTSET = mask; }
        static void outclr(uint8_t mask) { PORTK.OUTCLR = mask; }
        static void outtgl(uint8_t mask) { PORTK.OUTTGL = mask; }
    };

#endif

#if defined(PORTQ)

    template<>
    struct traits<Q> {
        static volatile uint8_t& dir() { return PORTQ.DIR; }
        static void dirset(uint8_t mask) { PORTQ.DIRSET = mask; }
        static void dirclr(uint8_t mask) { PORTQ.DIRCLR = mask; }
        static void dirtgl(uint8_t mask) { PORTQ.DIRTGL = mask; }
        static uint8_t in() { return PORTQ.IN; }
        static volatile uint8_t& out() { return PORTQ.OUT; }
        static void outset(uint8_t mask) { PORTQ.OUTSET = mask; }
        static void outclr(uint8_t mask) { PORTQ.OUTCLR = mask; }
        static void outtgl(uint8_t mask) { PORTQ.OUTTGL = mask; }
    };

#endif

} // namespace port

} // namespace artl

