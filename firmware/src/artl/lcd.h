
#pragma once

#include <stdint.h>

namespace artl {

template<typename D0_PIN, typename D1_PIN,
         typename D2_PIN, typename D3_PIN>
struct writer4 {
    using d0_pin = D0_PIN;
    using d1_pin = D1_PIN;
    using d2_pin = D2_PIN;
    using d3_pin = D3_PIN;

    void setup() {
        d0_pin().output();
        d1_pin().output();
        d2_pin().output();
        d3_pin().output();
    }

    void write(uint8_t v) {
        d0_pin().write(v & (1 << 0));
        d1_pin().write(v & (1 << 1));
        d2_pin().write(v & (1 << 2));
        d3_pin().write(v & (1 << 3));
    }
};

template<typename A0_PIN, typename EN_PIN,
    typename WRITER4,
    uint8_t COLS = 16, uint8_t ROWS = 2, uint8_t CSIZE = 0 /* 5x8DOTS */>
struct lcd {

    // commands
    enum {
        CLEARDISPLAY   = 0x01,
        RETURNHOME     = 0x02,
        ENTRYMODESET   = 0x04,
        DISPLAYCONTROL = 0x08,
        CURSORSHIFT    = 0x10,
        FUNCTIONSET    = 0x20,
        SETCGRAMADDR   = 0x40,
        SETDDRAMADDR   = 0x80,
    };

    // flags for display entry mode
    enum {
        ENTRYRIGHT          = 0x00,
        ENTRYLEFT           = 0x02,
        ENTRYSHIFTINCREMENT = 0x01,
        ENTRYSHIFTDECREMENT = 0x00,
    };

    // flags for display on/off control
    enum {
        DISPLAYON  = 0x04,
        DISPLAYOFF = 0x00,
        CURSORON   = 0x02,
        CURSOROFF  = 0x00,
        BLINKON    = 0x01,
        BLINKOFF   = 0x00,
    };

    // flags for display/cursor shift
    enum {
        DISPLAYMOVE = 0x08,
        CURSORMOVE  = 0x00,
        MOVERIGHT   = 0x04,
        MOVELEFT    = 0x00,
    };

    // flags for function set
    enum {
        _8BITMODE = 0x10,
        _4BITMODE = 0x00,
        _2LINE    = 0x08,
        _1LINE    = 0x00,
        _5x10DOTS = 0x04,
        _5x8DOTS  = 0x00,
    };

    using a0_pin = A0_PIN;
    using en_pin = EN_PIN;

    using writer4 = WRITER4;

    enum {
        DF = _4BITMODE | _1LINE | _5x8DOTS
                 | (ROWS > 1 ? _2LINE : 0)
                 | (ROWS == 1 && CSIZE != _5x8DOTS ? _5x10DOTS : _5x8DOTS)
    };

    void setup() {
        a0_pin().output();
        en_pin().output();

        writer4().setup();

        a0_pin().low();

        delayMicroseconds(20 * 1000);

        for (int i = 0; i < 3; i++) {
            write4(0x03, 40);
        }

        write4(0x02, 40);

        // finally, set # lines, font size, etc.
        command(FUNCTIONSET | DF);  
        // delayMicroseconds(40);

        display_control = 0;
        display();

        clear();

        // Initialize to default text direction (for romance languages)
        display_mode = ENTRYLEFT | ENTRYSHIFTDECREMENT;
        command(ENTRYMODESET | display_mode);
    }

    void command(uint8_t v, unsigned long d = 40) {
        a0_pin().low();

        write4(v >> 4);
        write4(v, d);
    }

    size_t write(uint8_t v) {
        a0_pin().high();

        write4(v >> 4);
        write4(v, 40);

        return 1;
    }


    void clear() {
        command(CLEARDISPLAY, 1500);
    }

    void home() {
        command(RETURNHOME);
    }

    void setCursor(uint8_t col, uint8_t row) {
        if (col > COLS) { col = COLS - 1; }
        if (row > ROWS) { row = ROWS - 1; }

        command(SETDDRAMADDR | (col + row ? 0x40 : 0x00));
    }


    void noDisplay() {
        display_control &= ~DISPLAYON;
        command(DISPLAYCONTROL | display_control);
    }

    void display() {
        display_control |= DISPLAYON;
        command(DISPLAYCONTROL | display_control);
    }


    void noCursor() {
        display_control &= ~CURSORON;
        command(DISPLAYCONTROL | display_control);
    }

    void cursor() {
        display_control |= CURSORON;
        command(DISPLAYCONTROL | display_control);
    }


    void noBlink() {
        display_control &= ~BLINKON;
        command(DISPLAYCONTROL | display_control);
    }

    void blink() {
        display_control |= BLINKON;
        command(DISPLAYCONTROL | display_control);
    }


    void scrollDisplayLeft() {
        command(CURSORSHIFT | DISPLAYMOVE);
    }

    void scrollDisplayRight() {
        command(CURSORSHIFT | DISPLAYMOVE | MOVERIGHT);
    }


    void leftToRight() {
        display_mode |= ENTRYLEFT;
        command(ENTRYMODESET | display_mode);
    }

    void rightToLeft() {
        display_mode &= ~ENTRYLEFT;
        command(ENTRYMODESET | display_mode);
    }


    void autoscroll() {
        display_mode |= ENTRYSHIFTDECREMENT;
        command(ENTRYMODESET | display_mode);
    }

    void noAutoscroll() {
        display_mode &= ~ENTRYSHIFTDECREMENT;
        command(ENTRYMODESET | display_mode);
    }


    void write4(uint8_t v) {
        en_pin().high();

        writer4().write(v);

        delayMicroseconds(1);
        en_pin().low();
    }

    void write4(uint8_t v, unsigned long d) {
        write4(v);

        delayMicroseconds(d);
    }

    void print(unsigned long n, uint8_t base = 10) {
      char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
      char *end = &buf[sizeof(buf) - 1];
      char *str = end;

      *str = '\0';

      // prevent crash if called with base == 1
      if (base < 2) base = 10;

      do {
        char c = n % base;
        n /= base;

        *--str = c < 10 ? c + '0' : c + 'A' - 10;
      } while(n);

      while (str < end) {
          write(*str);
      }
    }

    void write(const char* buf, uint8_t size) {
        const char *end = buf + size;

        while (buf < end) {
            write(*buf);
        }
    }

    template<typename T>
    void print(const T& t) {
        write((const char*) &t, sizeof(t));
    }

    uint8_t display_mode = 0;
    uint8_t display_control = 0;
};

}
