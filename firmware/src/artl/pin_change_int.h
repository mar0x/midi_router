
#pragma once

#include <stdint.h>
#include "bits/pin_change_traits.h"

namespace artl {

namespace pin {

template<
  typename PORT, uint8_t BIT_NO,
  typename TRAITS = change_traits<PORT, BIT_NO> >
struct change_int {

    using traits = TRAITS;

    static void enable() {
        icr_enable();
        traits::enable();
    }

    static void disable() { traits::disable(); }

    static void icr_enable() { PCICR |= traits::icr_bit_mask; }

    static void icr_disable() { PCICR &= ~traits::icr_bit_mask; }
};

}

}
