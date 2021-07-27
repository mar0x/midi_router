
#pragma once

#include <stdint.h>

namespace artl {

struct lsb_first {

    template<typename F>
    static void write(uint8_t v, const F&) {
        for(uint8_t i = 0; i < 8; i++) {
            F::write( (v & (1 << i)) != 0 );
        }
    }

    template<typename F>
    static void write(uint16_t v, const F& f) {
        write( (uint8_t) (v & 0xFF), f );
        write( (uint8_t) ((v >> 8) & 0xFF), f );
    }

    template<typename F>
    static void read(uint8_t& v, const F&) {
        uint8_t tmp = 0;
        for(uint8_t i = 0; i < 8; i++) {
            if (F::read()) {
                tmp |= (1 << i);
            }
        }

        v = tmp;
    }

    template<typename F>
    static void read(uint16_t& v, const F& f) {
        uint8_t lo, hi;
        read(lo, f);
        read(hi, f);
        v = lo | (hi << 8);
    }

};


struct lsb_first_fast {

    template<typename F>
    static void write(uint8_t v, const F&) {
        F::write( (v & 0x01u) != 0 );
        F::write( (v & 0x02u) != 0 );
        F::write( (v & 0x04u) != 0 );
        F::write( (v & 0x08u) != 0 );
        F::write( (v & 0x10u) != 0 );
        F::write( (v & 0x20u) != 0 );
        F::write( (v & 0x40u) != 0 );
        F::write( (v & 0x80u) != 0 );
    }

    template<typename F>
    static void write(uint16_t v, const F& f) {
        write( (uint8_t) ((v >> 8) & 0xFF), f );
        write( (uint8_t) (v & 0xFF), f );
    }

    template<typename F>
    static void read(uint8_t& v, const F&) {
        v =
            F::read() |
            (F::read() << 1) |
            (F::read() << 2) |
            (F::read() << 3) |
            (F::read() << 4) |
            (F::read() << 5) |
            (F::read() << 6) |
            (F::read() << 7);
    }

    template<typename F>
    static void read(uint16_t& v, const F& f) {
        uint8_t lo, hi;
        read(lo, f);
        read(hi, f);
        v = lo | (hi << 8);
    }

};


struct msb_first {

    template<typename F>
    static void write(uint8_t v, const F&) {
        for(uint8_t i = 7; i < 8; --i) {
            F::write( (v & (1 << i)) != 0 );
        }
    }

    template<typename F>
    static void write(uint16_t v, const F& f) {
        write( (uint8_t) ((v >> 8) & 0xFF), f );
        write( (uint8_t) (v & 0xFF), f );
    }

    template<typename F>
    static void read(uint8_t& v, const F&) {
        uint8_t tmp = 0;

        for(uint8_t i = 7; i < 8; --i) {
            if (F::read()) {
                tmp |= (1 << i);
            }
        }

        v = tmp;
    }

    template<typename F>
    static void read(uint16_t& v, const F& f) {
        uint8_t lo, hi;
        read( hi, f );
        read( lo, f );
        v = lo | (hi << 8);
    }

};


struct msb_first_fast {

    template<typename F>
    static void write(uint8_t v, const F&) {
        F::write( (v & 0x80u) != 0 );
        F::write( (v & 0x40u) != 0 );
        F::write( (v & 0x20u) != 0 );
        F::write( (v & 0x10u) != 0 );
        F::write( (v & 0x08u) != 0 );
        F::write( (v & 0x04u) != 0 );
        F::write( (v & 0x02u) != 0 );
        F::write( (v & 0x01u) != 0 );
    }

    template<typename F>
    static void write(uint16_t v, const F& f) {
        write( (uint8_t) ((v >> 8) & 0xFF), f );
        write( (uint8_t) (v & 0xFF), f );
    }

    template<typename F>
    static void read(uint8_t& v, const F& f) {
        v =
            (F::read() << 7) |
            (F::read() << 6) |
            (F::read() << 5) |
            (F::read() << 4) |
            (F::read() << 3) |
            (F::read() << 2) |
            (F::read() << 1) |
            F::read();
    }

    template<typename F>
    static void read(uint16_t& v, const F& f) {
        uint8_t lo, hi;
        read( hi, f );
        read( lo, f );
        v = lo | (hi << 8);
    }

};


}
