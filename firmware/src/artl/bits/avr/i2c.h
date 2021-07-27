
#pragma once

/* Arduino SSD1306Ascii Library
 * Copyright (C) 2015 by William Greiman
 *
 * This file is part of the Arduino SSD1306Ascii Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SSD1306Ascii Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
/**
 * @file AvrI2c.h
 * @brief Small fast I2C class for AVR.
 */

#include <stdint.h>
#include <avr/io.h>

namespace artl {

/**
 * \class AvrI2c
 * \brief Hardware I2C master class for AVR.
 *
 * Uses ATmega TWI hardware port
 */
struct i2c {
    enum {
        /** Bit to or with address for read start and read restart */
        I2C_READ = 1,

        /** Bit to or with address for write start and write restart */
        I2C_WRITE = 0
    };

    //------------------------------------------------------------------------------
    // Status codes in TWSR - names are from Atmel TWSR.h with TWSR_ added
    enum {

        /** start condition transmitted */
        TWSR_START = 0x08,

        /** repeated start condition transmitted */
        TWSR_REP_START = 0x10,

        /** slave address plus write bit transmitted, ACK received */
        TWSR_MTX_ADR_ACK = 0x18,

        /** data transmitted, ACK received */
        TWSR_MTX_DATA_ACK = 0x28,

        /** slave address plus read bit transmitted, ACK received */
        TWSR_MRX_ADR_ACK = 0x40,
    };
    //------------------------------------------------------------------------------

    /**
     * @brief Initialize prescalar and SLC clock rate.
     * @param[in] fast_mode Fast 400 kHz mode if true else standard 100 kHz mode.
     */
    static void begin(bool fast_mode = true) {
        // Zero prescaler.
        TWSR = 0;
        // Set bit rate.
        set_clock(fast_mode && F_CPU > 15000000 ? 400000 : 100000);
    }

    /**
     * @brief Read a byte and send Ack if more reads follow else
               Nak to terminate read.
     *
     * @param[in] last Set true to terminate the read else false.
     * @return The byte read from the I2C bus.
     */
    static uint8_t read(bool last) {
        exec_cmd((1 << TWINT) | (1 << TWEN) | (last ? 0 : (1 << TWEA)));
        return TWDR;
    }

    /**
     * @brief Issue a repeated start condition.
     *
     * same as start with no stop.  Included to document intention.
     *
     * @param[in] addr I2C address with read/write bit.
     * @return The value true, 1, for success or false, 0, for failure.
     */
    static bool repeated_start(uint8_t addr) {
        return start(addr);
    }

    /**
     * @brief Set the I2C bit rate.
     *
     * @param[in] frequency Desired frequency in Hz.
     *            Valid range for a 16 MHz board is about 40 kHz to 444,000 kHz.
     */
    static void set_clock(uint32_t frequency) {
        TWBR = ((F_CPU / frequency) - 16) / 2;
    }

    /**
     * @brief Issue a start condition.
     *
     * @param[in] addr I2C address with read/write bit.
     *
     * @return The value true for success or false for failure.
     */
    static bool start(uint8_t addr) {
        // send START condition
        uint8_t status = exec_cmd((1 << TWINT) | (1 << TWSTA) | (1 << TWEN));
        if (status != TWSR_START && status != TWSR_REP_START) {
            return false;
        }
        // send device address and direction
        TWDR = addr;
        status = exec_cmd((1 << TWINT) | (1 << TWEN));
        if (addr & I2C_READ) {
            return status == TWSR_MRX_ADR_ACK;
        } else {
            return status == TWSR_MTX_ADR_ACK;
        }
    }

    /** Issue a stop condition. */
    static void stop(void) {
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

        // wait until stop condition is executed and bus released
        while (TWCR & (1 << TWSTO)) {}
    }

    /**
     * @brief Write a byte.
     *
     * @param[in] data The byte to send.
     *
     * @return The value true, 1, if the slave returned an Ack or false for Nak.
     */
    static bool write(uint8_t data) {
        TWDR = data;
        uint8_t status = exec_cmd((1 << TWINT) | (1 << TWEN));
        return status == TWSR_MTX_DATA_ACK;
    }

private:
    static uint8_t exec_cmd(uint8_t cmdReg) {
        // send command
        TWCR = cmdReg;
        // wait for command to complete
        while (!(TWCR & (1 << TWINT))) {}
        // status bits.
        return TWSR & 0xF8;
    }
};

}
