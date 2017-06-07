/*
    Copyright (C) 2016-2017 Alexey Dynda

    This file is part of SSD1306 library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @file ssd1306_i2c.h SSD1306 i2c communication functions
 */


#ifndef _SSD1306_I2C_H_
#define _SSD1306_I2C_H_

#include <Arduino.h>

// Uncomment this block only, if you need to use "embedded" I2C on Atmega controllers
// #define SSD1306_EMBEDDED_I2C

#ifndef SSD1306_SA
    /**
     * SSD1306_SA defines I2C address of LCD display. Please, check your device.
     * If you LCD device has different address, change it here.
     * Write command will be SSD1306_SA<<1 and read will be SSD1306_SA<<1 | 1
     */
    #define SSD1306_SA    0x3C  // Slave address
#endif

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#ifndef SSD1306_EMBEDDED_I2C
    #define SSD1306_EMBEDDED_I2C
#endif
#endif


/**
 * Informs I2C device about data to be sent
 */
void         ssd1306_i2cStart();

/**
 * Completes I2C transaction
 */
void         ssd1306_i2cStop();

/**
 * Sends byte to I2C device
 * @param data - byte to send
 */
void         ssd1306_i2cSendByte(uint8_t data);

/**
 * Sends command to I2C device: includes initiating of
 * I2C transaction, sending data and completing I2C transaction.
 * @param data - command to send
 */
void         ssd1306_i2cSendCommand(uint8_t data);

/**
 * Starts I2C transaction for sending bitmap data.
 */
void         ssd1306_i2cDataStart();

// ----------------------------------------------------------------------------
#endif // _SSD1306_I2C_H_
