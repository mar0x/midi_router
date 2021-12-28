
#pragma once

#include <stdint.h>
#include <string.h>

enum {
    CMD_BIT           = 0x80U,

    CMD_NOTE_OFF      = 0x80U,
    CMD_NOTE_ON       = 0x90U,
    CMD_KEY_PRESSURE  = 0xA0U,
    CMD_CTRL_CHANGE   = 0xB0U,
    CMD_PROG_CHANGE   = 0xC0U,
    CMD_CHAN_PRESSURE = 0xD0U,
    CMD_PITCH_CHANGE  = 0xE0U,

    CMD_SYS           = 0xF0U,

    CMD_SYS_EX        = 0xF0U,
    CMD_SYS_EX_END    = 0xF7U,

    CMD_SYS_MTC       = 0xF1U,
    CMD_SYS_SONG_PP   = 0xF2U,
    CMD_SYS_SONG_SEL  = 0xF3U,
    CMD_SYS_TUNE_REQ  = 0xF6U,

    CMD_SYS_RT        = 0xF8U,

    CMD_SYS_CLOCK     = 0xF8U,
    CMD_SYS_TICK      = 0xF9U,
    CMD_SYS_START     = 0xFAU,
    CMD_SYS_CONT      = 0xFBU,
    CMD_SYS_STOP      = 0xFCU,
    CMD_SYS_UNDEF     = 0xFDU,
    CMD_SYS_ACTIVE_S  = 0xFEU,
    CMD_SYS_RESET     = 0xFFU,
};

enum {
    CTRL_ALL_SOUND_OFF  = 120,
    CTRL_RESET_ALL      = 121,
    CTRL_LOCAL_CONTROL  = 122,
};

inline bool is_midi_rt(uint8_t b) {
    return (b & CMD_SYS_RT) == CMD_SYS_RT;
}

inline bool is_midi_cmd(uint8_t b) {
    return (b & CMD_BIT) == CMD_BIT;
}

inline bool is_midi_sys(uint8_t b) {
    return (b & CMD_SYS) == CMD_SYS;
}

struct midi_cmd_t {
    midi_cmd_t() = default;
    midi_cmd_t(uint8_t ch, uint8_t cmd) :
        size_(1)
    {
        cmd_[0] = cmd | (ch & 0x0FU);
    }

    midi_cmd_t(uint8_t ch, uint8_t cmd, uint8_t val) :
        size_(2)
    {
        cmd_[0] = cmd | (ch & 0x0FU);
        cmd_[1] = val & 0x7FU;
    }

    midi_cmd_t(uint8_t ch, uint8_t cmd, uint8_t ctl, uint8_t val) :
        size_(3)
    {
        cmd_[0] = cmd | (ch & 0x0FU);
        cmd_[1] = ctl & 0x7FU;
        cmd_[2] = val & 0x7FU;
    }

    static uint8_t command(uint8_t b) {
        if (is_midi_cmd(b)) {
            uint8_t c = b & CMD_SYS;
            return c != CMD_SYS ? c : b;
        }

        return 0;
    }

    bool sys_ex() const { return sys_ex_ || (size_ > 0 && command() == CMD_SYS_EX); }
    void sys_ex(bool v) { sys_ex_ = v; }

    uint8_t command() const { return command(cmd_[0]); }
    uint8_t channel() const { return cmd_[0] & 0x0FU; }
    uint8_t controller() const { return cmd_[1]; }
    uint8_t value() const { return cmd_[2]; }
    uint8_t program() const { return cmd_[1]; }
    uint8_t key() const { return cmd_[1]; }
    uint16_t pitch() const { return cmd_[1] | (cmd_[2] << 7); }

    uint8_t size() const { return size_; }

    static uint8_t cmd_size(uint8_t b) {
        switch (command(b)) {
        case CMD_NOTE_OFF:
        case CMD_NOTE_ON:
        case CMD_KEY_PRESSURE:
        case CMD_CTRL_CHANGE:
        case CMD_PITCH_CHANGE:
        case CMD_SYS_SONG_PP:
            return 3;

        case CMD_PROG_CHANGE:
        case CMD_CHAN_PRESSURE:
        case CMD_SYS_MTC:
        case CMD_SYS_SONG_SEL:
        case CMD_SYS_EX:
            return 2;

        case CMD_SYS_EX_END:
        case CMD_SYS_CLOCK:
        case CMD_SYS_TICK:
        case CMD_SYS_START:
        case CMD_SYS_CONT:
        case CMD_SYS_STOP:
        case CMD_SYS_UNDEF:
        case CMD_SYS_ACTIVE_S:
        case CMD_SYS_RESET:
            return 1;
        }

        return 0;
    }

    bool ready() const {
        if (size_ == 0) return false;
        if (size_ == MAX_SIZE) return true;

        if (is_midi_cmd(cmd_[0]) && cmd_[0] != CMD_SYS_EX) {
            return size_ == cmd_size(cmd_[0]);
        }

        return cmd_[size_ - 1] == CMD_SYS_EX_END;
    }

    operator const uint8_t*() const { return cmd_; }
    operator uint8_t*() { return cmd_; }

    bool operator==(const midi_cmd_t& cmd) const {
        return size_ == cmd.size_ && memcmp(cmd_, cmd.cmd_, size_) == 0;
    }

    midi_cmd_t &operator<<(uint8_t b) {
        read(b);
        return *this;
    }

    void reset() {
        if (size_ == MAX_SIZE && command() == CMD_SYS_EX && cmd_[size_ - 1] != CMD_SYS_EX_END) {
            sys_ex_ = true;
        }

        if (size_ > 0 && cmd_[size_ - 1] == CMD_SYS_EX_END) {
            sys_ex_ = false;
        }

        size_ = 0;
    }

    void read(uint8_t b) {
        if (is_midi_cmd(b) && (b != CMD_SYS_EX_END || size_ == 0)) {
            cmd_[0] = b;
            size_ = 1;
            sys_ex_ = false;

            return;
        }

        if ((size_ > 0 || sys_ex_) && size_ < MAX_SIZE) {
            cmd_[size_++] = b;
        }
    }

private:
    enum {
        MAX_SIZE = 3
    };

    uint8_t cmd_[MAX_SIZE];
    uint8_t size_ = 0;
    bool sys_ex_ = false;
};
