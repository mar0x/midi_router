
#pragma once

#include <string.h>

#define FW_VERSION "2022.11.05Ex"

struct version_t {
    enum {
        MAX_LINE = 5,
    };

    const char *lines[MAX_LINE];

    static char sn_[];
    static char hw_[];

    version_t() {
        lines[0] = "FW " FW_VERSION;
        lines[1] = hw_;
        lines[2] = sn_;
        lines[3] = "BD " __DATE__;
        lines[4] = "BT " __TIME__;
    }

    void sn(const void *sn) {
        memcpy(sn_ + 3, sn, 16);
    }

    const char *sn() const { return sn_ + 3; }

    void hw(const char *hw) {
        memcpy(hw_ + 3, hw, 10);
    }

    const char *hw() const { return hw_ + 3; }
};

char version_t::sn_[] = "SN XXXXxxxxXXXXxxxx";
char version_t::hw_[] = "HW XXXX.XX.XX";
