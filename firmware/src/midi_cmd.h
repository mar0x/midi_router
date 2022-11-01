
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
    CMDID_NONE          =  0,

    CMDID_NOTE_OFF      =  1,   // CMD_NOTE_OFF      = 0x80U,
    CMDID_NOTE_ON       =  2,   // CMD_NOTE_ON       = 0x90U,
    CMDID_KEY_PRESSURE  =  3,   // CMD_KEY_PRESSURE  = 0xA0U,
    CMDID_CTRL_CHANGE   =  4,   // CMD_CTRL_CHANGE   = 0xB0U,
    CMDID_PROG_CHANGE   =  5,   // CMD_PROG_CHANGE   = 0xC0U,
    CMDID_CHAN_PRESSURE =  6,   // CMD_CHAN_PRESSURE = 0xD0U,
    CMDID_PITCH_CHANGE  =  7,   // CMD_PITCH_CHANGE  = 0xE0U,

    CMDID_SYS           =  8,   // CMD_SYS           = 0xF0U

    CMDID_SYS_EX        =  8,   // CMD_SYS_EX        = 0xF0U
    CMDID_SYS_EX_END    = 15,   // CMD_SYS_EX_END    = 0xF7U,

    CMDID_SYS_MTC       =  9,   // CMD_SYS_MTC       = 0xF1U,
    CMDID_SYS_SONG_PP   = 10,   // CMD_SYS_SONG_PP   = 0xF2U,
    CMDID_SYS_SONG_SEL  = 11,   // CMD_SYS_SONG_SEL  = 0xF3U,
    CMDID_SYS_TUNE_REQ  = 14,   // CMD_SYS_TUNE_REQ  = 0xF6U,

    CMDID_SYS_RT        = 16,   // CMD_SYS_CLOCK     = 0xF8U

    CMDID_SYS_CLOCK     = 16,   // CMD_SYS_CLOCK     = 0xF8U,
    CMDID_SYS_TICK      = 17,   // CMD_SYS_TICK      = 0xF9U,
    CMDID_SYS_START     = 18,   // CMD_SYS_START     = 0xFAU,
    CMDID_SYS_CONT      = 19,   // CMD_SYS_CONT      = 0xFBU,
    CMDID_SYS_STOP      = 20,   // CMD_SYS_STOP      = 0xFCU,
    CMDID_SYS_UNDEF     = 21,   // CMD_SYS_UNDEF     = 0xFDU,
    CMDID_SYS_ACTIVE_S  = 22,   // CMD_SYS_ACTIVE_S  = 0xFEU,
    CMDID_SYS_RESET     = 23,   // CMD_SYS_RESET     = 0xFFU,
};

enum {
    CMDMASK_NONE         = 0x00000000UL,
    CMDMASK_ALL          = 0x00FFFFFFUL,

    CMDBIT_NOTE_OFF      = (1UL << CMDID_NOTE_OFF     ),
    CMDBIT_NOTE_ON       = (1UL << CMDID_NOTE_ON      ),
    CMDBIT_KEY_PRESSURE  = (1UL << CMDID_KEY_PRESSURE ),
    CMDBIT_CTRL_CHANGE   = (1UL << CMDID_CTRL_CHANGE  ),
    CMDBIT_PROG_CHANGE   = (1UL << CMDID_PROG_CHANGE  ),
    CMDBIT_CHAN_PRESSURE = (1UL << CMDID_CHAN_PRESSURE),
    CMDBIT_PITCH_CHANGE  = (1UL << CMDID_PITCH_CHANGE ),

    CMDBIT_SYS_EX        = (1UL << CMDID_SYS_EX       ),
    CMDBIT_SYS_EX_END    = (1UL << CMDID_SYS_EX_END   ),

    CMDBIT_SYS_MTC       = (1UL << CMDID_SYS_MTC      ),
    CMDBIT_SYS_SONG_PP   = (1UL << CMDID_SYS_SONG_PP  ),
    CMDBIT_SYS_SONG_SEL  = (1UL << CMDID_SYS_SONG_SEL ),
    CMDBIT_SYS_TUNE_REQ  = (1UL << CMDID_SYS_TUNE_REQ ),

    CMDMASK_SYS          = CMDBIT_SYS_EX |
                           CMDBIT_SYS_EX_END |
                           CMDBIT_SYS_MTC |
                           CMDBIT_SYS_SONG_PP |
                           CMDBIT_SYS_SONG_SEL |
                           CMDBIT_SYS_TUNE_REQ,

    CMDBIT_SYS_CLOCK     = (1UL << CMDID_SYS_CLOCK    ),
    CMDBIT_SYS_TICK      = (1UL << CMDID_SYS_TICK     ),
    CMDBIT_SYS_START     = (1UL << CMDID_SYS_START    ),
    CMDBIT_SYS_CONT      = (1UL << CMDID_SYS_CONT     ),
    CMDBIT_SYS_STOP      = (1UL << CMDID_SYS_STOP     ),
    CMDBIT_SYS_UNDEF     = (1UL << CMDID_SYS_UNDEF    ),
    CMDBIT_SYS_ACTIVE_S  = (1UL << CMDID_SYS_ACTIVE_S ),
    CMDBIT_SYS_RESET     = (1UL << CMDID_SYS_RESET    ),

    CMDMASK_SYS_RT       = CMDBIT_SYS_CLOCK    |
                           CMDBIT_SYS_TICK     |
                           CMDBIT_SYS_START    |
                           CMDBIT_SYS_CONT     |
                           CMDBIT_SYS_STOP     |
                           CMDBIT_SYS_UNDEF    |
                           CMDBIT_SYS_ACTIVE_S |
                           CMDBIT_SYS_RESET,
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

    midi_cmd_t(uint8_t cmd) : size_(1) {
        cmd_[0] = cmd;
    }

    midi_cmd_t(uint8_t ch, uint8_t cmd) : size_(1) {
        cmd_[0] = cmd | (ch & 0x0FU);
    }

    midi_cmd_t(uint8_t ch, uint8_t cmd, uint8_t val) : size_(2) {
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

    bool sys_active_state() const {
        return size_ == 1 && cmd_[0] == CMD_SYS_ACTIVE_S;
    }

    uint8_t command() const { return command(cmd_[0]); }
    uint8_t channel() const { return cmd_[0] & 0x0FU; }
    uint8_t controller() const { return cmd_[1]; }
    uint8_t value() const { return cmd_[2]; }
    uint8_t program() const { return cmd_[1]; }
    uint8_t key() const { return cmd_[1]; }
    uint16_t pitch() const { return cmd_[1] | (cmd_[2] << 7); }

    uint8_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    bool full() const { return size_ == MAX_SIZE; };

    static uint8_t cmdid2size(uint8_t id) {
        static const uint8_t cmd2size[] = {
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

        return cmd2size[id];
    }

    static uint8_t cmdid2hdr(uint8_t id) {
        static const uint8_t cmd2hdr[] = {
            0,
          0x8, // CMD_NOTE_OFF      = 0x80U,
          0x9, // CMD_NOTE_ON       = 0x90U,
          0xA, // CMD_KEY_PRESSURE  = 0xA0U,
          0xB, // CMD_CTRL_CHANGE   = 0xB0U,
          0xC, // CMD_PROG_CHANGE   = 0xC0U,
          0xD, // CMD_CHAN_PRESSURE = 0xD0U,
          0xE, // CMD_PITCH_CHANGE  = 0xE0U,
         0xFF, // CMD_SYS_EX        = 0xF0U,
          0x2, // CMD_SYS_MTC       = 0xF1U,
          0x3, // CMD_SYS_SONG_PP   = 0xF2U,
          0x2, // CMD_SYS_SONG_SEL  = 0xF3U,
            0, // ? = 0xF4U,
            0, // ? = 0xF5U,
          0x5, // CMD_SYS_TUNE_REQ  = 0xF6U,
          0x5, // CMD_SYS_EX_END    = 0xF7U,
          0xF, // CMD_SYS_CLOCK     = 0xF8U,
          0xF, // CMD_SYS_TICK      = 0xF9U,
          0xF, // CMD_SYS_START     = 0xFAU,
          0xF, // CMD_SYS_CONT      = 0xFBU,
          0xF, // CMD_SYS_STOP      = 0xFCU,
          0xF, // CMD_SYS_UNDEF     = 0xFDU,
          0xF, // CMD_SYS_ACTIVE_S  = 0xFEU,
          0xF, // CMD_SYS_RESET     = 0xFFU,
        };

        return cmd2hdr[id];
    }

    static uint8_t cmd_size(uint8_t b) {
        // ((b & 0x70) >> 4) + ((b & CMD_SYS) == CMD_SYS) * (b & 0x0F)
/*
        static const uint8_t cmd2size[] = {
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
*/

        uint8_t n = is_midi_cmd(b)
            * (1 + ((b & 0x70) >> 4) + is_midi_sys(b) * (b & 0x0F));

        return cmdid2size(n);
    }

    operator const uint8_t*() const { return cmd_; }
    operator uint8_t*() { return cmd_; }

    bool operator==(const midi_cmd_t& cmd) const {
        return size_ == cmd.size_ && memcmp(cmd_, cmd.cmd_, size_) == 0;
    }

    void reset() {
        size_ = 0;
    }

    void append(uint8_t b) {
        cmd_[size_++] = b;
    }

private:
    enum {
        MAX_SIZE = 3
    };

    uint8_t cmd_[MAX_SIZE];
    uint8_t size_;
};
