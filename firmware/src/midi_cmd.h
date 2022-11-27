
#pragma once

#include <stdint.h>
#include <string.h>

enum {
    CMD_BIT           = 0x80U,

    CMD_NOTE_OFF      = 0x80U,  // #  1
    CMD_NOTE_ON       = 0x90U,  // #  2
    CMD_KEY_PRESSURE  = 0xA0U,  // #  3
    CMD_CTRL_CHANGE   = 0xB0U,  // #  4
    CMD_PROG_CHANGE   = 0xC0U,  // #  5
    CMD_CHAN_PRESSURE = 0xD0U,  // #  6
    CMD_PITCH_CHANGE  = 0xE0U,  // #  7

    CMD_SYS           = 0xF0U,

    CMD_SYS_EX        = 0xF0U,  // #  8
    CMD_SYS_EX_END    = 0xF7U,  // # 15

    CMD_SYS_MTC       = 0xF1U,  // #  9
    CMD_SYS_SONG_PP   = 0xF2U,  // # 10
    CMD_SYS_SONG_SEL  = 0xF3U,  // # 11
    CMD_SYS_TUNE_REQ  = 0xF6U,  // # 14

    CMD_SYS_RT        = 0xF8U,

    CMD_SYS_CLOCK     = 0xF8U,  // # 16
    CMD_SYS_TICK      = 0xF9U,  // # 17
    CMD_SYS_START     = 0xFAU,  // # 18
    CMD_SYS_CONT      = 0xFBU,  // # 19
    CMD_SYS_STOP      = 0xFCU,  // # 20
    CMD_SYS_UNDEF     = 0xFDU,  // # 21
    CMD_SYS_ACTIVE_S  = 0xFEU,  // # 22
    CMD_SYS_RESET     = 0xFFU,  // # 23
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

inline uint8_t midi_cmd_serial(uint8_t b) {
    if ((b & CMD_BIT) == 0) return 0;

    uint8_t res = 1 + ((b & 0x70) >> 4);

    if ((b & CMD_SYS) != CMD_SYS) return res;

    return res + (b & 0x0F);
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
        return is_midi_cmd(b)
            * ((b & 0xF0) + is_midi_sys(b) * (b & 0x0F));
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
        // ((b & 0x70) >> 4) + ((b & CMD_SYS) == CMD_SYS) * (b & 0x0F)
        static uint8_t cmd2size[] = {
            0,
            3, // CMD_NOTE_OFF      = 0x80U,
            3, // CMD_NOTE_ON       = 0x90U,
            3, // CMD_KEY_PRESSURE  = 0xA0U,
            3, // CMD_CTRL_CHANGE   = 0xB0U,
            2, // CMD_PROG_CHANGE   = 0xC0U,
            2, // CMD_CHAN_PRESSURE = 0xD0U,
            3, // CMD_PITCH_CHANGE  = 0xE0U,
         0xFF, // CMD_SYS_EX        = 0xF0U,
            2, // CMD_SYS_MTC       = 0xF1U,
            3, // CMD_SYS_SONG_PP   = 0xF2U,
            2, // CMD_SYS_SONG_SEL  = 0xF3U,
            0, // ? = 0xF4U,
            0, // ? = 0xF5U,
            1, // CMD_SYS_TUNE_REQ  = 0xF6U,
            1, // CMD_SYS_EX_END    = 0xF7U,
            1, // CMD_SYS_CLOCK     = 0xF8U,
            1, // CMD_SYS_TICK      = 0xF9U,
            1, // CMD_SYS_START     = 0xFAU,
            1, // CMD_SYS_CONT      = 0xFBU,
            1, // CMD_SYS_STOP      = 0xFCU,
            1, // CMD_SYS_UNDEF     = 0xFDU,
            1, // CMD_SYS_ACTIVE_S  = 0xFEU,
            1, // CMD_SYS_RESET     = 0xFFU,
        };

        uint8_t n = is_midi_cmd(b)
            * (1 + ((b & 0x70) >> 4) + is_midi_sys(b) * (b & 0x0F));

        return cmd2size[n];
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

enum {
    NOTE_C2 = 36,
    NOTE_Cs2,
    NOTE_Df2 = NOTE_Cs2,
    NOTE_D2,
    NOTE_Ds2,
    NOTE_Ef2 = NOTE_Ds2,
    NOTE_E2,
    NOTE_F2,
    NOTE_Fs2,
    NOTE_Gf2 = NOTE_Fs2,
    NOTE_G2,
    NOTE_Gs2,
    NOTE_Af2 = NOTE_Gs2,
    NOTE_A2,
    NOTE_As2,
    NOTE_B2,
    NOTE_H2 = NOTE_B2,
    NOTE_C3,
    NOTE_Cs3,
    NOTE_Df3 = NOTE_Cs3,
    NOTE_D3,
    NOTE_Ds3,
    NOTE_Ef3 = NOTE_Ds3,
    NOTE_E3,
    NOTE_F3,
    NOTE_Fs3,
    NOTE_Gf3 = NOTE_Fs3,
    NOTE_G3,
    NOTE_Gs3,
    NOTE_Af3 = NOTE_Gs3,
    NOTE_A3,
    NOTE_As3,
    NOTE_B3,
    NOTE_H3 = NOTE_B3,
    NOTE_C4,
    NOTE_Cs4,
    NOTE_Df4 = NOTE_Cs4,
    NOTE_D4,
    NOTE_Ds4,
    NOTE_Ef4 = NOTE_Ds4,
    NOTE_E4,
    NOTE_F4,
    NOTE_Fs4,
    NOTE_Gf4 = NOTE_Fs4,
    NOTE_G4,
    NOTE_Gs4,
    NOTE_Af4 = NOTE_Gs4,
    NOTE_A4,
    NOTE_As4,
    NOTE_B4,
    NOTE_H4 = NOTE_B4,
    NOTE_C5,
    NOTE_Cs5,
    NOTE_Df5 = NOTE_Cs5,
    NOTE_D5,
    NOTE_Ds5,
    NOTE_Ef5 = NOTE_Ds5,
    NOTE_E5,
    NOTE_F5,
    NOTE_Fs5,
    NOTE_Gf5 = NOTE_Fs5,
    NOTE_G5,
    NOTE_Gs5,
    NOTE_Af5 = NOTE_Gs5,
    NOTE_A5,
    NOTE_As5,
    NOTE_B5,
    NOTE_H5 = NOTE_B5,
};
