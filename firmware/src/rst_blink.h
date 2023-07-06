
#pragma once

#include "ui.h"
#include "artl/timer.h"

namespace {

uint8_t rst_blink_count = 0;
artl::timer<> rst_blink;

void rst_blink_start(unsigned long t) {
    rst_blink_count = 0;
    ui::rst_blink_state.last_write = ui::pulse_state.last_write;
    ui::rst_blink_active = true;
    ui::rst_blink();
    rst_blink.schedule(t + 600);
}

void rst_blink_stop() {
    rst_blink.cancel();
    ui::pulse_state.last_write = ui::rst_blink_state.last_write;
    ui::rst_blink_active = false;
}

void rst_blink_update(unsigned long t) {
    if (!rst_blink.update(t)) return;

    rst_blink.schedule(t + 600);

    ui::rst_blink();

    rst_blink_count++;

    if (rst_blink_count >= 5) {
        RST.STATUS |= RST_SRF_bm;

        CCP = CCP_IOREG_gc; // see AU manual, sect 3.14.1 (protected I/O)
        WDT.CTRL = WDT_ENABLE_bm | WDT_CEN_bm | WDT_PER_8CLK_gc;

        for (;;) ;
    }
}

}
