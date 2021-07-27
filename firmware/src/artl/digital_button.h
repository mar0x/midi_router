
#pragma once

#include "button.h"

namespace artl {

template<typename INPUT_PIN,
         typename HANDLERS = default_button_handlers,
         typename TIME_TRAITS = default_button_time_traits>
struct digital_button : public button<HANDLERS, TIME_TRAITS> {

    using base = button<HANDLERS, TIME_TRAITS>;

    using base::base;

    using input_pin = INPUT_PIN;

    void setup() {
        input_pin::input();
    }

    void update(unsigned long t) {
        base::update(input_pin::read(), t);
    }
};

}
