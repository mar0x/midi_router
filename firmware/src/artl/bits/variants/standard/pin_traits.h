
#pragma once

namespace artl {

namespace pin {
    namespace id {
        enum {
            pd0   = 0,
            rxd   = 0,
            rx    = 0,
            pd1   = 1,
            txd   = 1,
            tx    = 1,
            pd2   = 2,
            int0  = 2,
            pd3   = 3,
            int1  = 3,
            oc2b  = 3,
            pd4   = 4,
            t0    = 4,
            xck   = 4,
            pd5   = 5,
            t1    = 5,
            oc0b  = 5,
            pd6   = 6,
            ain0  = 6,
            oc0a  = 6,
            pd7   = 7,
            ain1  = 7,
            pb0   = 8,
            clko  = 8,
            icp1  = 8,
            pb1   = 9,
            oc1a  = 9,
            pb2   = 10,
            oc1b  = 10,
            ss_   = 10,
            pb3   = 11,
            oc2   = 11,
            mosi  = 11,
            pb4   = 12,
            miso  = 12,
            pb5   = 13,
            sck   = 13,

            pc0   = 14,
            a0    = 14,
            adc0  = 14,
            pc1   = 15,
            a1    = 15,
            adc1  = 15,
            pc2   = 16,
            a2    = 16,
            adc2  = 16,
            pc3   = 17,
            a3    = 17,
            adc3  = 17,
            pc4   = 18,
            a4    = 18,
            adc4  = 18,
            sda   = 18,
            pc5   = 19,
            a5    = 19,
            adc5  = 19,
            scl   = 19,

            micro_14 = 12,   mini_12 = micro_14,
            micro_15 = 13,   mini_13 = micro_15,
            micro_16 = 11,   mini_11 = micro_16,
            micro_18 = 14,   mini_14 = micro_18,
            micro_19 = 15,   mini_15 = micro_19,
            micro_20 = 16,   mini_16 = micro_20,
            micro_21 = 17,   mini_17 = micro_21,
        };
    }

    using oc0a = pin_traits<id::oc0a>;
    using oc0b = pin_traits<id::oc0b>;

    using oc1a = pin_traits<id::oc1a>;
    using oc1b = pin_traits<id::oc1b>;
};

template<>
struct pin_traits<0> : public base_pin_traits<port::D, 0> { };

template<>
struct pin_traits<1> : public base_pin_traits<port::D, 1> { };

template<>
struct pin_traits<2> : public base_pin_traits<port::D, 2> { };

template<>
struct pin_traits<3> : public base_pin_traits<port::D, 3> { };

template<>
struct pin_traits<4> : public base_pin_traits<port::D, 4> { };

template<>
struct pin_traits<5> : public base_pin_traits<port::D, 5> { };

template<>
struct pin_traits<6> : public base_pin_traits<port::D, 6> { };

template<>
struct pin_traits<7> : public base_pin_traits<port::D, 7> { };

template<>
struct pin_traits<8> : public base_pin_traits<port::B, 0> { };

template<>
struct pin_traits<9> : public base_pin_traits<port::B, 1> { };

template<>
struct pin_traits<10> : public base_pin_traits<port::B, 2> { };

template<>
struct pin_traits<11> : public base_pin_traits<port::B, 3> { };

template<>
struct pin_traits<12> : public base_pin_traits<port::B, 4> { };

template<>
struct pin_traits<13> : public base_pin_traits<port::B, 5> { };

template<>
struct pin_traits<14> : public base_pin_traits<port::C, 0> { };

template<>
struct pin_traits<15> : public base_pin_traits<port::C, 1> { };

template<>
struct pin_traits<16> : public base_pin_traits<port::C, 2> { };

template<>
struct pin_traits<17> : public base_pin_traits<port::C, 3> { };

template<>
struct pin_traits<18> : public base_pin_traits<port::C, 4> { };

template<>
struct pin_traits<19> : public base_pin_traits<port::C, 5> { };

}
