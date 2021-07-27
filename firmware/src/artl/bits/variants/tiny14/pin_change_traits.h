
#pragma once

namespace artl {

template<>
struct pin_change_traits<0> : public base_pin_change_traits<0, PCINT0> { };

template<>
struct pin_change_traits<1> : public base_pin_change_traits<0, PCINT1> { };

template<>
struct pin_change_traits<2> : public base_pin_change_traits<0, PCINT2> { };

template<>
struct pin_change_traits<3> : public base_pin_change_traits<0, PCINT3> { };

template<>
struct pin_change_traits<4> : public base_pin_change_traits<0, PCINT4> { };

template<>
struct pin_change_traits<5> : public base_pin_change_traits<0, PCINT5> { };

template<>
struct pin_change_traits<6> : public base_pin_change_traits<0, PCINT6> { };

template<>
struct pin_change_traits<7> : public base_pin_change_traits<0, PCINT7> { };

template<>
struct pin_change_traits<8> : public base_pin_change_traits<1, PCINT10> { };

template<>
struct pin_change_traits<9> : public base_pin_change_traits<1, PCINT9> { };

template<>
struct pin_change_traits<10> : public base_pin_change_traits<1, PCINT8> { };

}
