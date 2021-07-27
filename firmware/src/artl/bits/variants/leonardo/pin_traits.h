
#pragma once

namespace artl {

namespace pin {
    namespace id {
        enum {
            pd2   = 0,
            rxd1  = 0,
            pd3   = 1,
            pd1   = 2,
            sda   = 2,
            pd0   = 3,
            oc0b  = 3,
            scl   = 3,
            pd4   = 4,
            a6    = 4,
            pc6   = 5,
            oc3a  = 5,
            oc4a_ = 5,
            pd7   = 6,
            a7    = 6,
            oc4d  = 6,
            pe6   = 7,
            pb4   = 8,
            a8    = 8,
            pb5   = 9,
            a9    = 9,
            oc1a  = 9,
            adc12 = 9,
            oc4b_ = 9,
            pb6   = 10,
            a10   = 10,
            oc1b  = 10,
            adc13 = 10,
            oc4b  = 10,
            pb7   = 11,
            oc0a  = 11,
            oc1c  = 11,
            rts_  = 11,
            pd6   = 12,
            a11   = 12,
            oc4d_ = 12,
            adc9  = 12,
            t1    = 12,
            pc7   = 13,
            oc4a  = 13,
            icp3  = 13,
            clko  = 13,
            led   = 13,
/*
            pf7   = 14,
            a0    = 14,
            adc7  = 14,
            tdi   = 14,
            pf6   = 15,
            a1    = 15,
            adc6  = 15,
            tdo   = 15,
            pf5   = 16,
            a2    = 16,
            adc5  = 16,
            tms   = 16,
            pf4   = 17,
            a3    = 17,
            adc4  = 17,
            tck   = 17,
*/
            pb0   = 17,

            pf7   = 18,
            a0    = 18,
            pf6   = 19,
            a1    = 19,
            pf5   = 20,
            a2    = 20,
            pf4   = 21,
            a3    = 21,
            pf1   = 22,
            a4    = 22,
            pf0   = 23,
            a5    = 23,

            micro_14 = 14,   mini_12 = micro_14,
            micro_15 = 15,   mini_13 = micro_15,
            micro_16 = 16,   mini_11 = micro_16,
            micro_18 = 18,   mini_14 = micro_18,
            micro_19 = 19,   mini_15 = micro_19,
            micro_20 = 20,   mini_16 = micro_20,
            micro_21 = 21,   mini_17 = micro_21,

            pd5   = 30,
        };
    }

    using oc0a = pin_traits<id::oc0a>;
    using oc0b = pin_traits<id::oc0b>;

    using oc1a = pin_traits<id::oc1a>;
    using oc1b = pin_traits<id::oc1b>;

    using oc4a = pin_traits<id::oc4a>;
    using oc4b = pin_traits<id::oc4b>;
    using oc4d = pin_traits<id::oc4d>;
};

template<>
struct pin_traits<0> : public base_pin_traits<port::D, 2> { };

template<>
struct pin_traits<1> : public base_pin_traits<port::D, 3> { };

template<>
struct pin_traits<2> : public base_pin_traits<port::D, 1> { };

template<>
struct pin_traits<3> : public base_pin_traits<port::D, 0> { };

template<>
struct pin_traits<4> : public base_pin_traits<port::D, 4> { };

template<>
struct pin_traits<5> : public base_pin_traits<port::C, 6> { };

template<>
struct pin_traits<6> : public base_pin_traits<port::D, 7> { };

template<>
struct pin_traits<7> : public base_pin_traits<port::E, 6> { };

template<>
struct pin_traits<8> : public base_pin_traits<port::B, 4> { };

template<>
struct pin_traits<9> : public base_pin_traits<port::B, 5> { };

template<>
struct pin_traits<10> : public base_pin_traits<port::B, 6> { };

template<>
struct pin_traits<11> : public base_pin_traits<port::B, 7> { };

template<>
struct pin_traits<12> : public base_pin_traits<port::D, 6> { };

template<>
struct pin_traits<13> : public base_pin_traits<port::C, 7> { };

template<>
struct pin_traits<14> : public base_pin_traits<port::B, 3> { };

template<>
struct pin_traits<15> : public base_pin_traits<port::B, 1> { };

template<>
struct pin_traits<16> : public base_pin_traits<port::B, 2> { };

template<>
struct pin_traits<17> : public base_pin_traits<port::B, 0> { };

template<>
struct pin_traits<18> : public base_pin_traits<port::F, 7> { };

template<>
struct pin_traits<19> : public base_pin_traits<port::F, 6> { };

template<>
struct pin_traits<20> : public base_pin_traits<port::F, 5> { };

template<>
struct pin_traits<21> : public base_pin_traits<port::F, 4> { };

template<>
struct pin_traits<22> : public base_pin_traits<port::F, 1> { };

template<>
struct pin_traits<23> : public base_pin_traits<port::F, 0> { };

template<>
struct pin_traits<24> : public base_pin_traits<port::D, 4> { };

template<>
struct pin_traits<25> : public base_pin_traits<port::D, 7> { };

template<>
struct pin_traits<26> : public base_pin_traits<port::B, 4> { };

template<>
struct pin_traits<27> : public base_pin_traits<port::B, 5> { };

template<>
struct pin_traits<28> : public base_pin_traits<port::B, 6> { };

template<>
struct pin_traits<29> : public base_pin_traits<port::D, 6> { };

template<>
struct pin_traits<30> : public base_pin_traits<port::D, 5> { };

}
