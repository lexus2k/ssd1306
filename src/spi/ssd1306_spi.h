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
 * @file ssd1306_spi.h SSD1306 spi communication functions
 */

#ifndef _SSD1306_SPI_H_
#define _SSD1306_SPI_H_

#include "ssd1306_spi_conf.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_INTERFACE_API LCD communication interface functions
 * @{
 */

/** chip enable pin to controll lcd display over spi */
extern int8_t s_ssd1306_cs;
/** data/command control pin for spi interface of lcd display */
extern int8_t s_ssd1306_dc;

/**
 * Inits lcd interface to use hardware spi for communication.
 * The function automatically selects available type of spi implementation
 * 1. SPI library (ssd1306_spiInit_hw())
 * @param cesPin - pin, controlling chip enable of LCD
 * @param dcPin - pin, controlling data/command mode of LCD
 */
void         ssd1306_spiInit(int8_t cesPin, int8_t dcPin);

/**
 * Switches spi chip to command transmission mode.
 */
void         ssd1306_spiCommandStart();

/**
 * Switches spi chip to data transmission mode.
 */
void         ssd1306_spiDataStart();

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_SPI_H_
