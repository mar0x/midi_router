
#pragma once

#include <stdint.h>

namespace artl {

template<int N>
struct uart_traits {
    volatile uint8_t *ubrr();
    volatile uint8_t *ucsr();
    volatile uint8_t *udr();
};

#if defined(UBRRL)
#warning UBRRL defined
template<>
struct uart_traits<0> {
    volatile uint8_t *ubrr() { return &UBRRL; }
    volatile uint8_t *ucsr() { return &UCSRA; }
    volatile uint8_t *udr() { return &UDR; }
};
#endif

#if defined(UBRR0L)
#warning UBRR0L defined
template<>
struct uart_traits<0> {
    volatile uint8_t *ubrr() { return &UBRR0L; }
    volatile uint8_t *ucsr() { return &UCSR0A; }
    volatile uint8_t *udr() { return &UDR0; }
};
#endif

#if defined(UBRR1L)
#warning UBRR1L defined
template<>
struct uart_traits<1> {
    volatile uint8_t *ubrr() { return &UBRR1L; }
    volatile uint8_t *ucsr() { return &UCSR1A; }
    volatile uint8_t *udr() { return &UDR1; }
};
#endif

template<typename T>
struct uart {
    using traits = T;
};

}
