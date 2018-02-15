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
 * @file ssd1306_spi_linux.h SSD1306 spi communication functions for standard Linux SPI interface
 */

#ifndef _SSD1306_SPI_LINUX_H_
#define _SSD1306_SPI_LINUX_H_

#include "hal/io.h"
#include "ssd1306_spi_conf.h"

#ifdef SSD1306_LINUX_SUPPORTED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_INTERFACE_API LCD communication interface functions
 * @{
 */

/**
 * Inits lcd interface to use  SPI for communication.
 * It uses standard MOSI, SCLK pins to send data to LCD.
 * @param cesPin - pin, controlling chip enable of LCD
 * @param dcPin - pin, controlling data/command mode of LCD
 *
 * @note: after call to this function you need to initialize lcd display.
 * @warning: Nowadays this communication works only in SDL Emulation mode!
 */
void ssd1306_spiInit_Linux(int8_t cesPin, int8_t dcPin);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

#endif /* _SSD1306_SPI_LINUX_H_ */


