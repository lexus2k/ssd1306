/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

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
 * @file vga.h VGA basic data. Do not include this header in your project!!!
 */

#ifndef _SSD1306_VGA_H_
#define _SSD1306_VGA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ssd1306_hal/io.h"

#if defined(CONFIG_VGA_AVAILABLE) && defined(CONFIG_VGA_ENABLE)

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#if defined(__AVR_ATmega328P__)
/* TODO: Move defines out of this file */
static const uint8_t H_SYNC_PIN = 3;
static const uint8_t V_SYNC_PIN = 10;
extern volatile uint8_t __vga_buffer[];

/**
 * Make ms milliseconds delay. This function has very low precision: 16ms.
 *
 * @param ms time in milliseconds
 */
void ssd1306_vga_delay(uint32_t ms);

#elif defined(ESP32)

#endif

#endif // DOXYGEN_SHOULD_SKIP_THIS

/**
 * Prints vga buffer in text form using passed callback
 *
 * @param func callback to use for printing single character
 */
void ssd1306_debug_print_vga_buffer(void (*func)(uint8_t));

/**
 * Initializes hardware VGA controller
 * Be careful, this function reinitialized Atmega328p timers.
 * delay() function will not work after call.
 */
//void ssd1306_vga_controller_init(void);

#endif

/**
 *
 */
void ssd1306_vgaInit(void);

#ifdef __cplusplus
}
#endif

#endif

