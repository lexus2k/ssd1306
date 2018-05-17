/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

/**
 * @file ssd1306_spi.h SSD1306 spi communication functions
 */

#ifndef _SSD1306_SPI_H_
#define _SSD1306_SPI_H_

#include "ssd1306_hal/io.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup LCD_HW_INTERFACE_API
 *
 * chip enable pin to controll lcd display over spi
 */
extern int8_t s_ssd1306_cs;

/**
 * @ingroup LCD_HW_INTERFACE_API
 *
 * data/command control pin for spi interface of lcd display
 */
extern int8_t s_ssd1306_dc;

/**
 * @ingroup LCD_HW_INTERFACE_API
 * maximum SPI clock, supported by OLED display
 */
extern uint32_t s_ssd1306_spi_clock;

/**
 * @ingroup LCD_HW_INTERFACE_API
 *
 * Inits lcd interface to use hardware spi for communication.
 * The function automatically selects available type of spi implementation
 * 1. SPI library (ssd1306_spiInit_hw())
 * @param cesPin - pin, controlling chip enable of LCD
 * @param dcPin - pin, controlling data/command mode of LCD
 *
 * @note: after call to this function you need to initialize lcd display.
 */
void         ssd1306_spiInit(int8_t cesPin, int8_t dcPin);

/**
 * @ingroup LCD_HW_INTERFACE_API
 *
 * Switches spi to data(1) or command(0) mode.
 * @param mode - 1 data mode
 *               0 command mode
 */
void         ssd1306_spiDataMode(uint8_t mode);

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_SPI_H_
