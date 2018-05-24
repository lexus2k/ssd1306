/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

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
 * @file ssd1306_spi_usi.h SSD1306 SPI USI communication functions
 */

#ifndef _SSD1306_SPI_USI_H_
#define _SSD1306_SPI_USI_H_

#include "ssd1306_spi_conf.h"
#include "ssd1306_hal/io.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_USI_SPI_AVAILABLE) && defined(CONFIG_USI_SPI_ENABLE)

/**
 * @ingroup LCD_HW_INTERFACE_API
 *
 * Inits lcd interface to use USI for SPI communication.
 * It uses standard USI CLK, USI DO, USI DI pins to send data to LCD.
 * @param cesPin - pin, controlling chip enable of LCD
 * @param dcPin - pin, controlling data/command mode of LCD
 *
 * @note: after call to this function you need to initialize lcd display.
 */
void         ssd1306_spiInit_Usi(int8_t cesPin, int8_t dcPin);

#endif

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_SPI_USI_H_
