
#pragma once

namespace artl {

template<>
struct pin_change_traits<8> : public base_pin_change_traits<0, PCINT4> { };

template<>
struct pin_change_traits<9> : public base_pin_change_traits<0, PCINT5> { };

template<>
struct pin_change_traits<10> : public base_pin_change_traits<0, PCINT6> { };

template<>
struct pin_change_traits<11> : public base_pin_change_traits<0, PCINT7> { };

template<>
struct pin_change_traits<14> : public base_pin_change_traits<0, PCINT3> { };

template<>
struct pin_change_traits<15> : public base_pin_change_traits<0, PCINT1> { };

template<>
struct pin_change_traits<16> : public base_pin_change_traits<0, PCINT2> { };

}
