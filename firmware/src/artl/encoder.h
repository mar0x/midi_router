
#pragma once

#include "button.h"

namespace artl {

struct default_encoder_handler {
    void on_rotate(short, unsigned long) { }
};

struct default_encoder_time_traits {
    static constexpr unsigned long bounce() { return 2; }
    static constexpr unsigned long hold() { return 500; }
};

template<typename HANDLERS = default_encoder_handler,
         typename TIME_TRAITS = default_encoder_time_traits,
         short factor = 4>
struct encoder : public HANDLERS {

    using HANDLERS::HANDLERS;

    using time_traits = TIME_TRAITS;

/*
        a  b | ab
        ---------
        0, 0 |  0
        1, 0 |  2
        1, 1 |  3
        0, 1 |  1
        0, 0 |  0
*/

    const short p2d[4][4] = {
        {  0, -1,  1,  2 },
        {  1,  0,  2, -1 },
        { -1,  2,  0,  1 },
        {  2,  1, -1,  0 },
    };

    bool update(uint8_t a, uint8_t b, unsigned long t) {
        bool da = a_.update(a, t);
        bool db = b_.update(b, t);

        if (!da && !db) return false;

        uint8_t ab = (a_.down() << 1) | b_.down();
        short d = p2d[ab_][ab];

        ab_ = ab;

        // guess the direction
        if (d == 2 && d_ < 0) {
            d = -2;
        }

        // heuristic: previous guess was wrong
        if (d_ == -2 && d == 1 && pos_ == -2) {
            pos_ = 2;
        }

        if (d_ == 2 && d == -1 && pos_ == 2) {
            pos_ = -2;
        }

        d_ = d;
        pos_ += d;

        // TODO reset pos_ whan ab == 0

        while (pos_ >= factor) {
            pos_ -= factor;

            this->on_rotate(1, t);
        }

        while (pos_ <= -factor) {
            pos_ += factor;

            this->on_rotate(-1, t);
        }

        if (ab == 0) { pos_ = 0; }

        return true;
    }

    button<default_button_handler, time_traits> a_, b_;
    short pos_ = 0;
    short d_ = 0;
    uint8_t ab_ = 0;
};

}
