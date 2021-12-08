
#pragma once

#include <stdint.h>
#include <string.h>

struct serial_cmd_t {

    enum {
        CMD_UNKNOWN,
        CMD_VERSION,            // V
        CMD_MIDI_MON,           // M
        CMD_UPTIME,             // U
        CMD_PORT_STAT,          // P
        CMD_PORT_STAT_RESET,    // R
        CMD_HELP,               // ?

        CMD_SERIAL_NUMBER,      // S
        CMD_HARDWARE,           // H

        MAX_SIZE = 50,
        MAX_ARGS = 5,
    };

    serial_cmd_t() : ready_(false), size_(0), command_(CMD_UNKNOWN) { }

    void read(uint8_t b);

    operator bool() const { return ready_; }

    void reset() {
        ready_ = false;
        size_ = 0;
        command_ = CMD_UNKNOWN;
    }

    uint8_t command() const { return command_; }

    template<typename T>
    bool get_hex(uint8_t s, uint8_t e, T& v) {
        for (uint8_t p = s; p < e; ++p) {
            if (buf_[p] >= '0' && buf_[p] <= '9') {
                v = v * 16 + (buf_[p] - '0');
            } if (buf_[p] >= 'a' && buf_[p] <= 'f') {
                v = v * 16 + (10 + buf_[p] - 'a');
            } if (buf_[p] >= 'A' && buf_[p] <= 'F') {
                v = v * 16 + (10 + buf_[p] - 'A');
            } else {
                return false;
            }
        }

        return true;
    }

    template<typename T>
    bool get_dec(uint8_t s, uint8_t e, T& v) {
        for (uint8_t p = s; p < e; ++p) {
            if (buf_[p] >= '0' && buf_[p] <= '9') {
                v = v * 10 + (buf_[p] - '0');
            } else {
                return false;
            }
        }

        return true;
    }

    template<typename T>
    bool get_bin(uint8_t s, uint8_t e, T& v) {
        for (uint8_t p = s; p < e; ++p) {
            if (buf_[p] >= '0' && buf_[p] <= '1') {
                v = (v << 1) + (buf_[p] - '0');
            } else {
                return false;
            }
        }

        return true;
    }

    template<typename T>
    bool get_num(uint8_t n, T& v) {
        if (n >= arg_size_) {
            return false;
        }

        arg *a = &arg_[n];

        v = 0;

        if (a->end - a->start > 2) {
            if (buf_[a->start] == '0' && (buf_[a->start + 1] == 'x' || buf_[a->start + 1] == 'X')) {
                return get_hex(a->start + 2, a->end, v);
            }
        }

        if (a->end - a->start > 1) {
            if (buf_[a->end - 1] == 'h' || buf_[a->end - 1] == 'H') {
                return get_hex(a->start, a->end - 1, v);
            }

            if (buf_[a->end - 1] == 'b' || buf_[a->end - 1] == 'B') {
                return get_bin(a->start, a->end - 1, v);
            }
        }

        n = 1;

        for (uint8_t p = a->start; p < a->end; ++p) {
            if (buf_[p] >= '0' && buf_[p] <= '1') {
                v = v + ((buf_[p] - '0') ? n : 0);
                n = n << 1;
            } else {
                v = 0;
                break;
            }

            ++p;
            if (p >= a->end) return true;

            if (buf_[p] != ',') {
                v = 0;
                break;
            }
        }

        return get_dec(a->start, a->end, v);
    }

    bool get_arg(uint8_t n, uint8_t& v) {
        return get_num(n, v);
    }

    bool get_arg(uint8_t n, uint16_t& v) {
        return get_num(n, v);
    }

    bool get_arg(uint8_t n, uint32_t& v) {
        return get_num(n, v);
    }

    template<typename T>
    bool get_arg(uint8_t n, T& v) {
        if (n >= arg_size_) {
            return false;
        }

        uint8_t l = arg_[n].end - arg_[n].start;
        uint8_t c = (l > sizeof(T)) ? sizeof(T) : l;

        memcpy(&v, &buf_[arg_[n].start], c);

        if (l < sizeof(T)) {
            memset(((uint8_t *) &v) + l, ' ', sizeof(T) - l);
        }

        return true;
    }

//private:
    void parse();

    struct arg {
        uint8_t start;
        uint8_t end;
    };

    char buf_[MAX_SIZE];
    bool ready_;
    uint8_t size_;
    uint8_t command_;

    arg arg_[MAX_ARGS];
    uint8_t arg_size_;
};

inline void
serial_cmd_t::read(uint8_t b) {
    if (b == '\n' || b == '\r') {
        buf_[size_] = 0;

        parse();
        ready_ = true;

        return;
    }

    if (b == 127 && size_ != 0) {
        --size_;
        return;
    }

    if (size_ >= MAX_SIZE) {
        return;
    }

    buf_[size_++] = b;
}

inline void
serial_cmd_t::parse() {
    arg_size_ = 0;
    arg *a = &arg_[0];
    char in_quote = 0;

    a->start = 0;

    for (uint8_t i = 0; i < size_; i++) {
        char b = buf_[i];

        if (b == '"' && i == a->start) {
            a->start = i + 1;
            in_quote = b;
            continue;
        }

        if (in_quote && in_quote == b) {
            a->end = i;

            arg_size_++;
            a++;

            a->start = i + 1;
            in_quote = 0;

            if (arg_size_ >= MAX_ARGS) {
                return;
            }
        }

        if (in_quote == 0 && (b == ' ' || b == '\t')) {
            if (i == a->start) {
                a->start = i + 1;
                continue;
            }

            a->end = i;

            arg_size_++;
            a++;

            a->start = i + 1;

            if (arg_size_ >= MAX_ARGS) {
                return;
            }
        }
    }

    if (a->start < size_) {

        a->end = size_;

        arg_size_++;
    }

    if (arg_size_ > 0) {
        a = &arg_[0];
        char *b = &buf_[a->start];

        switch (a->end - a->start) {
        case 1:
            if (b[0] == '?') { command_ = CMD_HELP; }
            if (b[0] == 'H') { command_ = CMD_HARDWARE; }
            if (b[0] == 'M') { command_ = CMD_MIDI_MON; }
            if (b[0] == 'P') { command_ = CMD_PORT_STAT; }
            if (b[0] == 'R') { command_ = CMD_PORT_STAT_RESET; }
            if (b[0] == 'S') { command_ = CMD_SERIAL_NUMBER; }
            if (b[0] == 'U') { command_ = CMD_UPTIME; }
            if (b[0] == 'V') { command_ = CMD_VERSION; }
            break;
        }
    }
}

static PROGMEM_DECLARE(char, help_[]) = R"HELP(
V - show version
S - serial number
H - hardware revision

M - MIDI monitor
U - show uptime
P [s] [R] - port stats, every s sec, R - auto reset
R - reset port stats
)HELP";
