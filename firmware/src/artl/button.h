
#pragma once

namespace artl {

struct default_button_handler {
    void on_down(bool, unsigned long) { }
    void on_hold(unsigned long) { }
};


struct default_button_time_traits {
    static constexpr unsigned long bounce() { return 5; }
    static constexpr unsigned long hold() { return 500; }
};


template<typename HANDLERS = default_button_handler,
         typename TIME_TRAITS = default_button_time_traits>
struct button : public HANDLERS {

    using HANDLERS::HANDLERS;

    using time_traits = TIME_TRAITS;

    bool up() const { return !down_; }
    bool down() const { return down_; }

    bool hold() const { return hold_; }

    bool update(bool down, unsigned long t) {
        if (down != down_) {
            down_ = down;
            bounce_ = true;
            state_change_time_ = t;
        }

        if (!bounce_) {
            if (down_ && !hold_ &&
                t - state_change_time_ >= time_traits::hold())
            {
                hold_ = true;
                this->on_hold(t);
            }

            return false;
        }

        if (t - state_change_time_ >= time_traits::bounce()) {
            bounce_ = false;
            state_change_time_ = t;
            if (down_debounced_ != down_) {
                down_debounced_ = down_;
                this->on_down(down_, t);

                hold_ = false;

                return true;
            }
        }

        return false;
    }

private:
    bool down_ = false;
    bool down_debounced_ = false;
    bool bounce_ = false;
    bool hold_ = false;

    unsigned long state_change_time_ = 0;
};

}
