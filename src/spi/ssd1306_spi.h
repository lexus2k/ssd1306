/*
    Copyright (C) 2017 Alexey Dynda

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


#ifndef _SSD1306_SPI_H_
#define _SSD1306_SPI_H_

#include "ssd1306_spi_conf.h"
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Inits lcd interface to use hardware SPI for communication.
 * It uses standard MOSI, SCLK pins to send data to LCD.
 * @param cesPin - pin, controlling chip enable of LCD
 * @param dcPin - pin, controlling data/command mode of LCD
 */
void         ssd1306_hwSpiInit(int8_t cesPin, int8_t dcPin);

/**
 * Informs SPI device about data to be sent
 */
void         ssd1306_spiStart();

/**
 * Completes SPI transaction
 */
void         ssd1306_spiStop();

/**
 * Sends byte to SPI device
 * @param data - byte to send
 */
void         ssd1306_spiSendByte(uint8_t data);

/**
 * Switches spi chip to command transmission mode.
 */
void         ssd1306_spiCommandStart();

/**
 * Switches spi chip to data transmission mode.
 */
void         ssd1306_spiDataStart();


#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_I2C_H_
