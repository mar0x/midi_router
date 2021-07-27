
#pragma once

namespace artl {

namespace pin {
    namespace id {
        enum {
            pa0     = 0,
            adc0    = 0,
            aref    = 0,
            pcint0  = 0,
            pa1     = 1,
            adc1    = 1,
            ain0    = 1,
            pcint1  = 1,
            pa2     = 2,
            adc2    = 2,
            ain1    = 2,
            pcint2  = 2,
            pa3     = 3,
            adc3    = 3,
            t0      = 3,
            pcint3  = 3,
            pa4     = 4,
            adc4    = 4,
            usck    = 4,
            scl     = 4,
            t1      = 4,
            pcint4  = 4,
            pa5     = 5,
            adc5    = 5,
            do_     = 5,
            miso    = 5,
            oc1b    = 5,
            pcint5  = 5,
            pa6     = 6,
            adc6    = 6,
            di      = 6,
            mosi    = 6,
            sda     = 6,
            oc1a    = 6,
            pcint6  = 6,
            pa7     = 7,
            adc7    = 7,
            oc0b    = 7,
            icp     = 7,
            pcint7  = 7,
            pb2     = 8,
            ckout   = 8,
            oc0a    = 8,
            int0    = 8,
            pcint10 = 8,
            pb1     = 9,
            xtal2   = 9,
            pcint9  = 9,
            pb0     = 10,
            clki    = 10,
            xtal1   = 10,
            pcint8  = 10,
        };
    }

    using oc0a = pin_traits<id::oc0a>;
    using oc0b = pin_traits<id::oc0b>;

    using oc1a = pin_traits<id::oc1a>;
    using oc1b = pin_traits<id::oc1b>;
};

template<>
struct pin_traits<0> : public base_pin_traits<port::A, 0> { };

template<>
struct pin_traits<1> : public base_pin_traits<port::A, 1> { };

template<>
struct pin_traits<2> : public base_pin_traits<port::A, 2> { };

template<>
struct pin_traits<3> : public base_pin_traits<port::A, 3> { };

template<>
struct pin_traits<4> : public base_pin_traits<port::A, 4> { };

template<>
struct pin_traits<5> : public base_pin_traits<port::A, 5> { };

template<>
struct pin_traits<6> : public base_pin_traits<port::A, 6> { };

template<>
struct pin_traits<7> : public base_pin_traits<port::A, 7> { };

template<>
struct pin_traits<8> : public base_pin_traits<port::B, 2> { };

template<>
struct pin_traits<9> : public base_pin_traits<port::B, 1> { };

template<>
struct pin_traits<10> : public base_pin_traits<port::B, 0> { };

}
