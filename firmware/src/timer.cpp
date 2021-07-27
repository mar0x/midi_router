#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>      // Include for sleep mode (see 'wait_for_interrupt()')

#include "crit_sec.h"
/*
#define clk2usec(a) ( (a) * 1000000L / (F_CPU) )
#define usec2clk(a) ( (a) * (F_CPU) / 1000000L )
*/

#define clk2usec(a) ( (a) * 1000L / (F_CPU / 1000L) )
#define usec2clk(a) ( (a) * (F_CPU / 1000L) / 1000L )

#if F_CPU == 32000000L || F_CPU == 24000000L
// setup timer to have 1 interrupt per 512 usec
// clk2usec(1) * presc * 256 = 512
#define tc_prescaler   64
#else
// setup timer to have 1 interrupt per 128 usec
// clk2usec(1) * presc * 256 = 128
#define tc_prescaler    8
#endif

#define tc_int2usec(a)  clk2usec(a * tc_prescaler * 256)

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (tc_int2usec(1) / 1000)

// NOTE:  xmega runs at 32 mhz typically.  However, it uses THE PERIPHERAL CLOCK
//        for the timer.  Normally this is the same as the CPU clock unless you use
//        some crazy clock pre-scaler.
//        See sections 6.9 and 6.10 in D manual for system clock setup

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz and 32 Mhz for xmega - this doesn't lose precision.)

#define FRACT_INC ((tc_int2usec(1) % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile uint8_t tc_flag = 0;

namespace {

volatile unsigned long tc_int_count = 0;
volatile unsigned long tc_millis = 0;
uint8_t tc_millis_fract = 0;

volatile unsigned long rtc_ovf_count = 0;

template<int P>
struct tc2_clksel_div { };

template<> struct tc2_clksel_div<1> { enum { gc = TC2_CLKSEL_DIV1_gc }; };
template<> struct tc2_clksel_div<2> { enum { gc = TC2_CLKSEL_DIV2_gc }; };
template<> struct tc2_clksel_div<4> { enum { gc = TC2_CLKSEL_DIV4_gc }; };
template<> struct tc2_clksel_div<8> { enum { gc = TC2_CLKSEL_DIV8_gc }; };
template<> struct tc2_clksel_div<64> { enum { gc = TC2_CLKSEL_DIV64_gc }; };
template<> struct tc2_clksel_div<256> { enum { gc = TC2_CLKSEL_DIV256_gc }; };
template<> struct tc2_clksel_div<1024> { enum { gc = TC2_CLKSEL_DIV1024_gc }; };


void timer_init(TC2_t *t)
{
    t->CTRLA = tc2_clksel_div<tc_prescaler>::gc; // AU manual 15.10.1
    t->CTRLB = 0; // compare outputs disabled on all 8 bits (15.10.2)
//  t->CTRLC = 0; // when timer not running, sets compare (15.10.3)
    t->CTRLE = TC2_BYTEM_SPLITMODE_gc; // 'split' mode - AU manual 15.10.4
    t->CTRLF = TC2_CMD_NONE_gc;   // not resetting or anything (15.10.7)

    t->LPER = 255; // count 255 to 0 (total period = 256)
//  t->HPER = 255;
    t->HPER = 15;

    // pre-assign comparison registers to 'zero' (for PWM out) which is actually 255
    // 'timer 2' counts DOWN.  This, however, would generate a '1' output.

    t->LCMPA = 255;
    t->LCMPB = 255;
    t->LCMPC = 255;
    t->LCMPD = 255;

    t->HCMPA = 255;
    t->HCMPB = 255;
    t->HCMPC = 255;
    t->HCMPD = 255;

    // disable underflow and comparison interrupts
    t->INTCTRLA = 0;   // no underflow interrupts
    t->INTCTRLB = 0;   // no comparison interrupts
}

}

ISR(TCD2_LUNF_vect)
{
    // for this to work the limit must be 255 (8-bit mode)

    // copy these to local variables so they can be stored in registers
    // (volatile variables must be read from memory on every access)
    unsigned long m = tc_millis;
    uint8_t f = tc_millis_fract;

    m += MILLIS_INC;
    f += FRACT_INC;

    if (f >= FRACT_MAX) {
        f -= FRACT_MAX;
        m += 1;
    }

    tc_millis_fract = f;
    tc_millis = m;

    ++tc_int_count;

    tc_flag = 1;
}

ISR(RTC_OVF_vect)
{
    ++rtc_ovf_count;
}

unsigned long millis_()
{
    return tc_millis;
}

unsigned long millis()
{
    crit_sec cs;

    unsigned long m = tc_millis;

    return m;
}

unsigned long millis_rtc_()
{
    unsigned long m = rtc_ovf_count * 1000;

    if (RTC.INTFLAGS & RTC_OVFIF_bm) {
        m += 1000;
        ++rtc_ovf_count;

        RTC.INTFLAGS |= RTC_OVFIF_bm;
    }

    m += RTC.CNT;

    return m;
}

unsigned long millis_rtc()
{
    crit_sec cs;

    return millis_rtc_();
}

unsigned long micros()
{
    crit_sec cs;

    unsigned long m = tc_int_count; // for xmega it's really an underflow except 'E' series
    uint8_t t = 255 - TCD2.LCNT; // 'low' count, it's what we interrupt on (and it always counts DOWN)
                         // must subtract count value from 255 for this to work correctly

    // check the interrupt flag to see if I just got an underflow

    if ((TCD2.INTFLAGS & TC2_LUNFIF_bm) && (t < 255)) { // which means I underflowed but didn't call the ISR yet
        ++m; // increment ISR count for more accurate microseconds
    }

    return ((m << 8) + t) * tc_prescaler / usec2clk(1); // TODO:  make the '64' a #define ?
}

// XMEGA-specific code
void yield()
{
    cli(); // disable interrupts

    set_sleep_mode(SLEEP_MODE_IDLE); // everything on but CPU and NVRAM

    sleep_enable();

    sei(); // re-enable interrupts

    sleep_cpu(); // go to sleep

    sleep_disable(); // first thing to do out of sleep
}

void timer_init()
{
    crit_sec cs;

    PR.PRPD &= ~PR_TC0_bm;

    timer_init(&TCD2);

    TCD2.INTCTRLA |= TC2_LUNFINTLVL_HI_gc; // enable LOW underflow interrupt, pri level 3 (see 15.10.5 in AU manual)
}
