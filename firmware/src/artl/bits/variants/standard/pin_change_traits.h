
#pragma once

namespace artl {

template<>
struct pin_change_traits<0> : public base_pin_change_traits<2, PCINT16> { };

template<>
struct pin_change_traits<1> : public base_pin_change_traits<2, PCINT17> { };

template<>
struct pin_change_traits<2> : public base_pin_change_traits<2, PCINT18> { };

template<>
struct pin_change_traits<3> : public base_pin_change_traits<2, PCINT19> { };

template<>
struct pin_change_traits<4> : public base_pin_change_traits<2, PCINT20> { };

template<>
struct pin_change_traits<5> : public base_pin_change_traits<2, PCINT21> { };

template<>
struct pin_change_traits<6> : public base_pin_change_traits<2, PCINT22> { };

template<>
struct pin_change_traits<7> : public base_pin_change_traits<2, PCINT23> { };

template<>
struct pin_change_traits<8> : public base_pin_change_traits<0, PCINT0> { };

template<>
struct pin_change_traits<9> : public base_pin_change_traits<0, PCINT1> { };

template<>
struct pin_change_traits<10> : public base_pin_change_traits<0, PCINT2> { };

template<>
struct pin_change_traits<11> : public base_pin_change_traits<0, PCINT3> { };

template<>
struct pin_change_traits<12> : public base_pin_change_traits<0, PCINT4> { };

template<>
struct pin_change_traits<13> : public base_pin_change_traits<0, PCINT5> { };

template<>
struct pin_change_traits<14> : public base_pin_change_traits<1, PCINT8> { };

template<>
struct pin_change_traits<15> : public base_pin_change_traits<1, PCINT9> { };

template<>
struct pin_change_traits<16> : public base_pin_change_traits<1, PCINT10> { };

template<>
struct pin_change_traits<17> : public base_pin_change_traits<1, PCINT11> { };

template<>
struct pin_change_traits<18> : public base_pin_change_traits<1, PCINT12> { };

template<>
struct pin_change_traits<19> : public base_pin_change_traits<1, PCINT13> { };

}
