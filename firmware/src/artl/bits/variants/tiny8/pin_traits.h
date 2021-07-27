
#pragma once

namespace artl {

namespace pin {
    namespace id {
        enum {
            pb0     = 0,
            mosi    = 0,
            di      = 0,
            sda     = 0,
            ain0    = 0,
            oc0a    = 0,
            oc1a_   = 0,
            aref    = 0,
            pcint0  = 0,
            pb1     = 1,
            miso    = 1,
            do_     = 1,
            ain1    = 1,
            oc0b    = 1,
            oc1a    = 1,
            pcint1  = 1,
            pb2     = 2,
            sck     = 2,
            usck    = 2,
            scl     = 2,
            adc1    = 2,
            t0      = 2,
            int0    = 2,
            pcint2  = 2,
            pb3     = 3,
            adc3    = 3,
            oc1b_   = 3,
            clki    = 3,
            xtal1   = 3,
            pcint3  = 3,
            pb4     = 4,
            adc2    = 4,
            oc1b    = 4,
            clko    = 4,
            xtal2   = 4,
            pcint4  = 4,
            pb5     = 5,
            dw      = 5,
            adc0    = 5,
            reset_  = 5,
            pcint5  = 5,
        };
    }

    using oc0a = pin_traits<id::oc0a>;
    using oc0b = pin_traits<id::oc0b>;

    using oc1a = pin_traits<id::oc1a>;
    using oc1b = pin_traits<id::oc1b>;
};

template<>
struct pin_traits<0> : public base_pin_traits<port::B, 0> { };

template<>
struct pin_traits<1> : public base_pin_traits<port::B, 1> { };

template<>
struct pin_traits<2> : public base_pin_traits<port::B, 2> { };

template<>
struct pin_traits<3> : public base_pin_traits<port::B, 3> { };

template<>
struct pin_traits<4> : public base_pin_traits<port::B, 4> { };

template<>
struct pin_traits<5> : public base_pin_traits<port::B, 5> { };

}
