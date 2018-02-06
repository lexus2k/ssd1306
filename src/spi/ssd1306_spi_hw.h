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
 * @file ssd1306_spi_hw.h SSD1306 HW spi communication functions
 */

#ifndef _SSD1306_HW_SPI_H_
#define _SSD1306_HW_SPI_H_

#include "ssd1306_spi_conf.h"
#include "hal/io.h"

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
 *
 * @note: after call to this function you need to initialize lcd display.
 */
void         ssd1306_spiInit_hw(int8_t cesPin, int8_t dcPin);

#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_HW_SPI_H_
