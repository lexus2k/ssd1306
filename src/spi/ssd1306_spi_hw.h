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
 * @file ssd1306_spi_hw.h SSD1306 HW spi communication functions
 */

#ifndef _SSD1306_HW_SPI_H_
#define _SSD1306_HW_SPI_H_

#include "ssd1306_spi_conf.h"
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_INTERFACE_API LCD communication interface functions
 * @{
 */

#ifdef SSD1306_SPI_SUPPORTED

/**
 * Initializes SPI library. Automatically called by ssd1306_spiInit().
 * You may skip this by using ssd1306_spiInit_hw() instead of ssd1306_spiInit().
 */
void         ssd1306_spiConfigure_hw();

/**
 * Inits lcd interface to use hardware SPI for communication (SPI.h library).
 * It uses standard MOSI, SCLK pins to send data to LCD.
 * @param cesPin - pin, controlling chip enable of LCD
 * @param dcPin - pin, controlling data/command mode of LCD
 */
void         ssd1306_spiInit_hw(int8_t cesPin, int8_t dcPin);

/**
 * Starts spi transmission to the device
 */
void         ssd1306_spiStart_hw();

/**
 * Completes spi transaction
 */
void         ssd1306_spiStop_hw();

/**
 * Sends byte to spi device.
 * @param data - byte to send
 */
void         ssd1306_spiSendByte_hw(uint8_t data);

#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_HW_SPI_H_
