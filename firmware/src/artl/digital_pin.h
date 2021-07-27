
#pragma once

#include <stdint.h>
#include "bits/pin_traits.h"

namespace artl {

template<
  typename PORT, uint8_t BIT_NO,
  typename TRAITS = pin::traits<PORT, BIT_NO> >
struct digital_pin {

    using traits = TRAITS;


    static void output() { traits::output(); }

    static void input() { traits::input(); }


    static void high() { traits::high(); }

    static void low() { traits::low(); }

    static void toggle() { traits::toggle(); }

    static void write(bool v) { traits::write(v); }

    static bool read() { return traits::read(); }

    static uint8_t read_bit() { return traits::read_bit(); }
};

}
