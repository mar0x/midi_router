
#pragma once

namespace artl {

struct default_timer_handler {
    void operator()(unsigned long) { }
};

template<typename CALLBACK = default_timer_handler>
struct timer : public CALLBACK {

    using CALLBACK::CALLBACK;

    constexpr unsigned long max_delay() const {
        return ((unsigned long) -1) / 2;
    }

    void schedule(unsigned long t) { at_ = t; }

    void cancel() { at_ = 0; }

    bool update(unsigned long t) {
        if (at_ != 0 && t - at_ < max_delay()) {
            at_ = 0;
            (*this)(t);

            return true;
        }

        return false;
    }

    bool active() const { return at_ != 0; }

private:
    unsigned long at_ = 0;
};

}
