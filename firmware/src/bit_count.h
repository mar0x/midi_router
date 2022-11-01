
#pragma once

#include <stdint.h>

template<uint8_t BYTES>
struct byte_count { };

template<> struct byte_count<1> { using type = uint8_t; };
template<> struct byte_count<2> { using type = uint16_t; };
template<> struct byte_count<3> { using type = uint32_t; };
template<> struct byte_count<4> { using type = uint32_t; };

template<uint8_t BITS>
struct bit_count {
    using type = typename byte_count<(BITS - 1) / 8 + 1>::type;
};

