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
 * @file ssd1306_fonts.h Fonts for monochrome/rgb oled display
 */

#ifndef SSD1306_FONTS_H
#define SSD1306_FONTS_H

#include "hal/io.h"

/**
 * @defgroup LCD_FONTS LCD Fonts
 * @{
 */

/** Standard ASCII 6x8 font */
extern const PROGMEM uint8_t ssd1306xled_font6x8 [];

/** Standard ASCII 6x8 AB font with only capital letters */
extern const PROGMEM uint8_t ssd1306xled_font6x8_AB [];

/** Standard ASCII 5x7 font */
extern const PROGMEM uint8_t ssd1306xled_font5x7 [];

/** Standard ASCII 5x7 font with only capital letters */
extern const PROGMEM uint8_t ssd1306xled_font5x7_AB [];

/**
 * Standard ASCII font 11x16 with digits only (Ascii codes 32 - 64).
 * @warning can be used only with ssd1306_printFixed() and ssd1306_setFixedFont() functions.
 */
extern const PROGMEM uint8_t courier_new_font11x16_digits [];

/**
 * @}
 */

#endif // SSD1306_FONTS_H
