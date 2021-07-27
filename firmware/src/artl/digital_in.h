
#pragma once

#include <stdint.h>

#include "bits/pin_traits.h"

namespace artl {

template<
  typename PORT, uint8_t BIT_NO,
  typename TRAITS = pin::traits<PORT, BIT_NO> >
struct digital_in {

    using traits = TRAITS;

    static void setup() { traits::input(); }

    static void input() { traits::input(); }

    static void pullup() { traits::high(); }

    static void pulldown() { traits::low(); }

    static bool read() { return traits::read(); }

    static uint8_t read_bit() { return traits::read_bit(); }
};

}
