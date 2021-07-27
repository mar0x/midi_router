
#pragma once

#include <stdint.h>
#include <avr/io.h>

#include "port.h"

namespace artl {

namespace pin {

    template<uint8_t N>
    struct change_mask {
    };

#if defined(PCMSK) && defined(PCIE)

    template<>
    struct change_mask<0> {
        static volatile uint8_t& msk() { return PCMSK; }

        constexpr static uint8_t icr_bit_mask = 1 << PCIE;
    };

#else

#if defined(PCMSK0) && defined(PCIE0)

    template<>
    struct change_mask<0> {
        static volatile uint8_t& msk() { return PCMSK0; }

        constexpr static uint8_t icr_bit_mask = 1 << PCIE0;
    };

#endif

#endif

#if defined(PCMSK1) && defined(PCIE1)

    template<>
    struct change_mask<1> {
        static volatile uint8_t& msk() { return PCMSK1; }

        constexpr static uint8_t icr_bit_mask = 1 << PCIE1;
    };

#endif

#if defined(PCMSK2) && defined(PCIE2)

    template<>
    struct change_mask<2> {
        static volatile uint8_t& msk() { return PCMSK2; }

        constexpr static uint8_t icr_bit_mask = 1 << PCIE2;
    };

#endif

    template<typename PORT, uint8_t BIT_NO>
    struct change_traits {
    };

    template<uint8_t MSK, uint8_t BIT_NO>
    struct base_change_traits {

        using mask = change_mask<MSK>;

        constexpr static uint8_t icr_bit_mask = mask::icr_bit_mask;

        constexpr static uint8_t bit_mask = 1 << BIT_NO;

        static void enable() { mask::msk() |= bit_mask; }

        static void disable() { mask::msk() &= ~bit_mask; }
    };

#if defined (__AVR_ATmega32U4__)
    template<>
    struct change_traits<port::B, 0> : public base_change_traits<0, PCINT0> { };

    template<>
    struct change_traits<port::B, 1> : public base_change_traits<0, PCINT1> { };

    template<>
    struct change_traits<port::B, 2> : public base_change_traits<0, PCINT2> { };

    template<>
    struct change_traits<port::B, 3> : public base_change_traits<0, PCINT3> { };

    template<>
    struct change_traits<port::B, 4> : public base_change_traits<0, PCINT4> { };

    template<>
    struct change_traits<port::B, 5> : public base_change_traits<0, PCINT5> { };

    template<>
    struct change_traits<port::B, 6> : public base_change_traits<0, PCINT6> { };

    template<>
    struct change_traits<port::B, 7> : public base_change_traits<0, PCINT7> { };
#endif

} // namespace pin

} // namespace artl
