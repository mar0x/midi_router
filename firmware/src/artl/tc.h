#pragma once

namespace artl {

template<int N>
struct tc {
};


// Timer Counter
#if defined(TIMSK) || defined(TIMSK0)

template<>
struct tc<0> {
#if defined(TIMSK)
    volatile uint8_t& imsk() { return TIMSK; }

    template<int B>
    struct imsk_bit {
        void enable() const { TIMSK |= (1 << B); }
        void disable() const { TIMSK &= ~(1 << B); }
    };
#else
    volatile uint8_t& imsk() { return TIMSK0; }

    template<int B>
    struct imsk_bit {
        void enable() const { TIMSK0 |= (1 << B); }
        void disable() const { TIMSK0 &= ~(1 << B); }
    };
#endif

    // Output Compare A Match Interrupt
    imsk_bit<OCIE0A> oca() const { return imsk_bit<OCIE0A>(); }

    // Output Compare B Match Interrupt 
    imsk_bit<OCIE0B> ocb() const { return imsk_bit<OCIE0B>(); }

    // Overflow Interrupt
    imsk_bit<TOIE0> overflow() const { return imsk_bit<TOIE0>(); }

    // Control Register A
    volatile uint8_t& cra() { return TCCR0A; }
    // Control Register B
    volatile uint8_t& crb() { return TCCR0B; }

    // Output Compare Register A
    volatile uint8_t& ocra() { return OCR0A; }

    // Output Compare Register B
    volatile uint8_t& ocrb() { return OCR0B; }

    // Counter Value
    volatile uint8_t& cnt() { return TCNT0; }

    // Clock Source
    struct cs {
        enum {
            stopped     = (0 << CS02) | (0 << CS01) | (0 << CS00),
            presc_1     = (0 << CS02) | (0 << CS01) | (1 << CS00),
            presc_8     = (0 << CS02) | (1 << CS01) | (0 << CS00),
            presc_64    = (0 << CS02) | (1 << CS01) | (1 << CS00),
            presc_256   = (1 << CS02) | (0 << CS01) | (0 << CS00),
            presc_1024  = (1 << CS02) | (0 << CS01) | (1 << CS00),
            ext_falling = (1 << CS02) | (1 << CS01) | (0 << CS00),
            ext_rising  = (1 << CS02) | (1 << CS01) | (1 << CS00),
        };
    };

    // Compare Output Mode
    struct com {
        enum {
            normal = 0x00, // Normal port operation, OC0A/OC0B disconnected.
            toggle = 0x01, // Toggle OC0A/OC0B on compare match.
            clear = 0x02, // Clear OC0A/OC0B on compare match (Set output to low level).
            set = 0x03, // Set OC0A/OC0B on compare match (Set output to high level).
        };
    };

    void setup(uint8_t com_a, uint8_t com_b, uint8_t wgm, uint8_t cs) {
        cra() = ( (com_a & 0x03) << COM0A0 ) |
                ( (com_b & 0x03) << COM0B0 ) |
                ( (wgm & 0x03) << WGM00 );
        crb() = ( ((wgm >> 2) & 0x01) << WGM02 ) |
                ( (cs & 0x07) << CS00 );
    }
};

#endif


#if defined(TIMSK)

// Timer Counter
template<>
struct tc<1> {
    volatile uint8_t& imsk() { return TIMSK; }

    template<int B>
    struct imsk_bit {
        void enable() const { TIMSK |= (1 << B); }
        void disable() const { TIMSK &= ~(1 << B); }
    };

    // Output Compare A Match Interrupt
    imsk_bit<OCIE1A> oca() const { return imsk_bit<OCIE1A>(); }

    // Output Compare B Match Interrupt 
    imsk_bit<OCIE1B> ocb() const { return imsk_bit<OCIE1B>(); }

    // Overflow Interrupt
    imsk_bit<TOIE1> overflow() const { return imsk_bit<TOIE1>(); }

    // Control Register
    volatile uint8_t& cr() { return TCCR1; }

    // General Control Register
    volatile uint8_t& gcr() { return GTCCR; }

    struct ocra_t {
        void operator=(uint8_t v) { OCR1A = v; }
        operator uint8_t() const { return OCR1A; }
    };

    // Output Compare Register A
    volatile uint8_t& ocra() { return OCR1A; }

    struct ocrb_t {
        void operator=(uint8_t v) { OCR1B = v; }
        operator uint8_t() const { return OCR1B; }
    };

    // Output Compare Register B
    volatile uint8_t& ocrb() { return OCR1B; }

    struct ocrc_t {
        void operator=(uint8_t v) { OCR1C = v; }
        operator uint8_t() const { return OCR1C; }
    };

    // Output Compare Register C
    volatile uint8_t& ocrc() { return OCR1C; }

    struct cnt_t {
        void operator=(uint8_t v) { TCNT1 = v; }
        operator uint8_t() const { return TCNT1; }
    };

    // Counter Value
    volatile uint8_t& cnt() { return TCNT1; }

    // Clock Source
    struct cs {
        enum {
            stopped     = (0 << CS13) | (0 << CS12) | (0 << CS11) | (0 << CS10),
            presc_1     = (0 << CS13) | (0 << CS12) | (0 << CS11) | (1 << CS10),
            presc_2     = (0 << CS13) | (0 << CS12) | (1 << CS11) | (0 << CS10),
            presc_4     = (0 << CS13) | (0 << CS12) | (1 << CS11) | (1 << CS10),
            presc_8     = (0 << CS13) | (1 << CS12) | (0 << CS11) | (0 << CS10),
            presc_16    = (0 << CS13) | (1 << CS12) | (0 << CS11) | (1 << CS10),
            presc_32    = (0 << CS13) | (1 << CS12) | (1 << CS11) | (0 << CS10),
            presc_64    = (0 << CS13) | (1 << CS12) | (1 << CS11) | (1 << CS10),
            presc_128   = (1 << CS13) | (0 << CS12) | (0 << CS11) | (0 << CS10),
            presc_256   = (1 << CS13) | (0 << CS12) | (0 << CS11) | (1 << CS10),
            presc_512   = (1 << CS13) | (0 << CS12) | (1 << CS11) | (0 << CS10),
            presc_1024  = (1 << CS13) | (0 << CS12) | (1 << CS11) | (1 << CS10),
            presc_2048  = (1 << CS13) | (1 << CS12) | (0 << CS11) | (0 << CS10),
            presc_4096  = (1 << CS13) | (1 << CS12) | (0 << CS11) | (1 << CS10),
            presc_8192  = (1 << CS13) | (1 << CS12) | (1 << CS11) | (0 << CS10),
            presc_16384 = (1 << CS13) | (1 << CS12) | (1 << CS11) | (1 << CS10),
        };
    };

    // Compare Output Mode
    struct com {
        enum {
            normal = 0x00, // Normal port operation, OC1A/OC1B disconnected.
            toggle = 0x01, // Toggle OC1A/OC1B on compare match.
            clear = 0x02, // Clear OC1A/OC1B on compare match (Set output to low level).
            set = 0x03, // Set OC1A/OC1B on compare match (Set output to high level).
        };
    };

    void setup(uint8_t com_a, uint8_t com_b, uint8_t /*wgm*/, uint8_t cs) {
        cr() = ( (com_a & 0x03) << COM1A0 ) |
                ( (cs & 0x0F) << CS10 );
        gcr() = ( (com_b & 0x03) << COM1B0 );
    }
};

#endif


#if defined(TIMSK1)

// Timer Counter
template<>
struct tc<1> {
    volatile uint8_t& imsk() { return TIMSK1; }

    template<int B>
    struct imsk_bit {
        void enable() const { TIMSK1 |= (1 << B); }
        void disable() const { TIMSK1 &= ~(1 << B); }
    };

    // Output Compare A Match Interrupt
    imsk_bit<OCIE1A> oca() const { return imsk_bit<OCIE1A>(); }

    // Output Compare B Match Interrupt 
    imsk_bit<OCIE1B> ocb() const { return imsk_bit<OCIE1B>(); }

    // Overflow Interrupt
    imsk_bit<TOIE1> overflow() const { return imsk_bit<TOIE1>(); }

    // Control Register A
    volatile uint8_t& cra() { return TCCR1A; }

    // Control Register B
    volatile uint8_t& crb() { return TCCR1B; }

    // Control Register C
    volatile uint8_t& crc() { return TCCR1C; }

    // Output Compare Register A
    volatile uint16_t& ocra() { return OCR1A; }
    volatile uint8_t& ocral() { return OCR1AL; }
    volatile uint8_t& ocrah() { return OCR1AH; }

    // Output Compare Register B
    volatile uint16_t& ocrb() { return OCR1B; }
    volatile uint8_t& ocrbl() { return OCR1BL; }
    volatile uint8_t& ocrbh() { return OCR1BH; }

    // Counter Value
    volatile uint16_t& cnt() { return TCNT1; }
    volatile uint8_t& cntl() { return TCNT1L; }
    volatile uint8_t& cnth() { return TCNT1H; }

    // Clock Source
    struct cs {
        enum {
            stopped     = (0 << CS12) | (0 << CS11) | (0 << CS10),
            presc_1     = (0 << CS12) | (0 << CS11) | (1 << CS10),
            presc_8     = (0 << CS12) | (1 << CS11) | (0 << CS10),
            presc_64    = (0 << CS12) | (1 << CS11) | (1 << CS10),
            presc_256   = (1 << CS12) | (0 << CS11) | (0 << CS10),
            presc_1024  = (1 << CS12) | (0 << CS11) | (1 << CS10),
            ext_falling = (1 << CS12) | (1 << CS11) | (0 << CS10),
            ext_rising  = (1 << CS12) | (1 << CS11) | (1 << CS10),
        };
    };

    // Compare Output Mode
    struct com {
        enum {
            normal = 0x00, // Normal port operation, OC1A/OC1B disconnected.
            toggle = 0x01, // Toggle OC1A/OC1B on compare match.
            clear = 0x02, // Clear OC1A/OC1B on compare match (Set output to low level).
            set = 0x03, // Set OC1A/OC1B on compare match (Set output to high level).
        };
    };

    void setup(uint8_t com_a, uint8_t com_b, uint8_t wgm, uint8_t cs) {
        cra() = ( (com_a & 0x03) << COM1A0 ) |
                ( (com_b & 0x03) << COM1B0 ) |
                ( (wgm & 0x03) << WGM10 );
        crb() = ( ((wgm >> 2) & 0x03) << WGM12 ) |
                ( (cs & 0x07) << CS10 );
    }
};

#endif


#if defined(TIMSK3)

template<>
struct tc<3> {
    volatile uint8_t& imsk() { return TIMSK3; }

    template<int B>
    struct imsk_bit {
        void enable() const { TIMSK3 |= (1 << B); }
        void disable() const { TIMSK3 &= ~(1 << B); }
    };

    // Output Compare A Match Interrupt
    imsk_bit<OCIE3A> oca() const { return imsk_bit<OCIE3A>(); }

    // Output Compare B Match Interrupt 
    imsk_bit<OCIE3B> ocb() const { return imsk_bit<OCIE3B>(); }

    // Overflow Interrupt
    imsk_bit<TOIE3> overflow() const { return imsk_bit<TOIE3>(); }

    // Control Register A
    volatile uint8_t& cra() { return TCCR3A; }
    // Control Register B
    volatile uint8_t& crb() { return TCCR3B; }
    // Control Register C
    volatile uint8_t& crc() { return TCCR3C; }

    // Output Compare Register A
    volatile uint16_t& ocra() { return OCR3A; }
    volatile uint8_t& ocral() { return OCR3AL; }
    volatile uint8_t& ocrah() { return OCR3AH; }

    // Output Compare Register B
    volatile uint16_t& ocrb() { return OCR3B; }
    volatile uint8_t& ocrbl() { return OCR3BL; }
    volatile uint8_t& ocrbh() { return OCR3BH; }

    // Counter Value
    volatile uint16_t& cnt() { return TCNT3; }
    volatile uint8_t& cntl() { return TCNT3L; }
    volatile uint8_t& cnth() { return TCNT3H; }

    // Clock Source
    struct cs {
        enum {
            stopped     = (0 << CS32) | (0 << CS31) | (0 << CS30),
            presc_1     = (0 << CS32) | (0 << CS31) | (1 << CS30),
            presc_8     = (0 << CS32) | (1 << CS31) | (0 << CS30),
            presc_64    = (0 << CS32) | (1 << CS31) | (1 << CS30),
            presc_256   = (1 << CS32) | (0 << CS31) | (0 << CS30),
            presc_1024  = (1 << CS32) | (0 << CS31) | (1 << CS30),
            ext_falling = (1 << CS32) | (1 << CS31) | (0 << CS30),
            ext_rising  = (1 << CS32) | (1 << CS31) | (1 << CS30),
        };
    };

    // Compare Output Mode
    struct com {
        enum {
            normal = 0x00, // Normal port operation, OC3A/OC3B disconnected.
            toggle = 0x01, // Toggle OC3A/OC3B on compare match.
            clear = 0x02, // Clear OC3A/OC3B on compare match (Set output to low level).
            set = 0x03, // Set OC3A/OC3B on compare match (Set output to high level).
        };
    };

    void setup(uint8_t com_a, uint8_t com_b, uint8_t wgm, uint8_t cs) {
        cra() = ( (com_a & 0x03) << COM3A0 ) |
                ( (com_b & 0x03) << COM3B0 ) |
                ( (wgm & 0x03) << WGM30 );
        crb() = ( ((wgm >> 2) & 0x03) << WGM32 ) |
                ( (cs & 0x07) << CS30 );
    }
};

#endif

#if defined(TIMSK4)

template<>
struct tc<4> {
    volatile uint8_t& imsk() { return TIMSK4; }

    template<int B>
    struct imsk_bit {
        void enable() const { TIMSK4 |= (1 << B); }
        void disable() const { TIMSK4 &= ~(1 << B); }
    };

    // Output Compare A Match Interrupt
    imsk_bit<OCIE4A> oca() const { return imsk_bit<OCIE4A>(); }

    // Output Compare B Match Interrupt 
    imsk_bit<OCIE4B> ocb() const { return imsk_bit<OCIE4B>(); }

    // Overflow Interrupt
    imsk_bit<TOIE4> overflow() const { return imsk_bit<TOIE4>(); }

    // Control Register A
    volatile uint8_t& cra() { return TCCR4A; }
    // Control Register B
    volatile uint8_t& crb() { return TCCR4B; }
    // Control Register C
    volatile uint8_t& crc() { return TCCR4C; }
    // Control Register D
    volatile uint8_t& crd() { return TCCR4D; }
    // Control Register E
    volatile uint8_t& cre() { return TCCR4E; }

    // Output Compare Register A
    volatile uint8_t& ocra() { return OCR4A; }

    // Output Compare Register B
    volatile uint8_t& ocrb() { return OCR4B; }

    // Output Compare Register C
    volatile uint8_t& ocrc() { return OCR4C; }

    // Output Compare Register D
    volatile uint8_t& ocrd() { return OCR4D; }

    // Counter Value
    volatile uint16_t& cnt() { return TCNT4; }
    volatile uint8_t& cntl() { return TCNT4L; }
    volatile uint8_t& cnth() { return TCNT4H; }

    // Clock Source
    struct cs {
        enum {
            stopped     = (0 << CS43) | (0 << CS42) | (0 << CS41) | (0 << CS40),
            presc_1     = (0 << CS43) | (0 << CS42) | (0 << CS41) | (1 << CS40),
            presc_2     = (0 << CS43) | (0 << CS42) | (1 << CS41) | (0 << CS40),
            presc_4     = (0 << CS43) | (0 << CS42) | (1 << CS41) | (1 << CS40),
            presc_8     = (0 << CS43) | (1 << CS42) | (0 << CS41) | (0 << CS40),
            presc_16    = (0 << CS43) | (1 << CS42) | (0 << CS41) | (1 << CS40),
            presc_32    = (0 << CS43) | (1 << CS42) | (1 << CS41) | (0 << CS40),
            presc_64    = (0 << CS43) | (1 << CS42) | (1 << CS41) | (1 << CS40),
            presc_128   = (1 << CS43) | (0 << CS42) | (0 << CS41) | (0 << CS40),
            presc_256   = (1 << CS43) | (0 << CS42) | (0 << CS41) | (1 << CS40),
            presc_512   = (1 << CS43) | (0 << CS42) | (1 << CS41) | (0 << CS40),
            presc_1024  = (1 << CS43) | (0 << CS42) | (1 << CS41) | (1 << CS40),
            presc_2048  = (1 << CS43) | (1 << CS42) | (0 << CS41) | (0 << CS40),
            presc_4096  = (1 << CS43) | (1 << CS42) | (0 << CS41) | (1 << CS40),
            presc_8192  = (1 << CS43) | (1 << CS42) | (1 << CS41) | (0 << CS40),
            presc_16384 = (1 << CS43) | (1 << CS42) | (1 << CS41) | (1 << CS40),
        };
    };

    // Compare Output Mode
    struct com {
        enum {
            normal = 0x00, // Normal port operation, OC4A/OC4B disconnected.
            toggle = 0x01, // Toggle OC4A/OC4B on compare match.
            clear = 0x02, // Clear OC4A/OC4B on compare match (Set output to low level).
            set = 0x03, // Set OC4A/OC4B on compare match (Set output to high level).
        };
    };

    void setup(uint8_t com_a, uint8_t com_b, uint8_t pwm, uint8_t cs) {
        cra() = ( (com_a & 0x03) << COM4A0 ) |
                ( (com_b & 0x03) << COM4B0 ) |
                ( (pwm & 0x03) << PWM4B );
        crb() = ( (cs & 0x0F) << CS40 );
    }
};

#endif

}
