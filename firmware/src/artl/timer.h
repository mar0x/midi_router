
#pragma once

namespace artl {

struct default_timer_handler {
    void operator()(unsigned long) { }
};

template<typename CALLBACK = default_timer_handler>
struct timer : public CALLBACK {

    using CALLBACK::CALLBACK;
    using time_type = unsigned long;

    constexpr time_type max_delay() const {
        return ((unsigned long) -1) / 2;
    }

    void schedule(time_type t) { at_ = t; }

    void cancel() { at_ = 0; }

    bool update(time_type t) {
        if (at_ != 0 && t - at_ < max_delay()) {
            at_ = 0;
            (*this)(t);

            return true;
        }

        return false;
    }

    bool active() const { return at_ != 0; }

    time_type at() const { return at_; }

private:
    time_type at_ = 0;
};

}
