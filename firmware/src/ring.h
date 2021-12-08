#pragma once

#include <stdint.h>

template<uint16_t N>
struct size_bytes {
    enum {
        value = (N <= 0xFF) ? 1 : 2,
    };
};

template<uint8_t BYTES>
struct uint_ { };

template<> struct uint_<1> { using type = uint8_t; };
template<> struct uint_<2> { using type = uint16_t; };

template<uint16_t MAX, typename T>
struct ring {
    using value_t = T;
    using index_t = typename uint_< size_bytes<MAX - 1>::value >::type;

    enum {
        capacity = MAX
    };

    bool empty() const { return begin == end; }
    bool full() const { return begin == (end + 1) % MAX; }
    index_t size() const { return (end + MAX - begin) % MAX; }

    void reset() {
        begin = 0;
        end = 0;
    }

    value_t front() const {
        return data[begin];
    }

    void push_back(value_t v) {
        data[end] = v;
        end = (end + 1) % MAX;
    }

    void back(value_t v) {
        data[(MAX + end - 1) % MAX] = v;
    }

    value_t pop_front() {
        value_t res = data[begin];
        begin = (begin + 1) % MAX;

        return res;
    }

    index_t begin = 0;
    index_t end = 0;
    value_t data[MAX];
};
