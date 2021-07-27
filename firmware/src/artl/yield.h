
#pragma once

#if defined(ARDUINO_ARCH_AVR)

#include <avr/interrupt.h>
#include <avr/sleep.h>

#endif

namespace artl
{

inline void yield()
{
#if defined(ARDUINO_ARCH_AVR)
    sleep_enable();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_cpu();
#endif
}

}
