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
    enum {
        capacity = MAX,
        DATA_SIZE = MAX + 1,
    };

    using value_t = T;
    using index_t = typename uint_< size_bytes<DATA_SIZE - 1>::value >::type;

    bool empty() const { return begin == end; }
    bool full() const { return begin == (end + 1) % DATA_SIZE; }
    index_t size() const { return (end + DATA_SIZE - begin) % DATA_SIZE; }
    void clear() { begin = end; }
    index_t avail() const { return capacity - size(); }

    void reset() {
        begin = 0;
        end = 0;
    }

    value_t front() const {
        return data[begin];
    }

    void push_back(value_t v) {
        data[end] = v;
        end = (end + 1) % DATA_SIZE;
    }

    void back(value_t v) {
        data[(DATA_SIZE + end - 1) % DATA_SIZE] = v;
    }

    value_t pop_front() {
        value_t res = data[begin];
        begin = (begin + 1) % DATA_SIZE;

        return res;
    }

    bool pop_front(value_t &res) {
        if (empty()) {
            return false;
        }

        res = pop_front();

        return true;
    }

    index_t begin;
    index_t end;
    value_t data[DATA_SIZE];
};

template<typename T>
struct ring<1, T> {
    using value_t = T;
    using index_t = uint8_t;

    enum {
        capacity = 1,
    };

    inline bool empty() const { return !valid; }
    inline bool full() const { return valid; }
    inline index_t size() const { return valid ? 1 : 0; }
    inline void clear() { valid = false; }
    inline index_t avail() const { return valid ? 0 : 1; }

    inline void reset() { valid = false; }

    inline value_t front() const { return data; }

    inline void push_back(value_t v) {
        data = v;
        valid = true;
    }

    inline void back(value_t v) { data = v; }

    inline value_t pop_front() {
        valid = false;
        return data;
    }

    inline bool pop_front(value_t &res) {
        res = data;
        return valid;
    }

    value_t data;
    bool valid;
};
