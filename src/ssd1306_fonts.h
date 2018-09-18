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

#include "ssd1306_hal/io.h"
#include "nano_gfx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_FONTS FONTS: Supported LCD fonts
 * @{
 */

/** Flag means that more chars are required to decode utf-8 */
#define SSD1306_MORE_CHARS_REQUIRED  0xffff

/** Standard ASCII 6x8 font */
extern const PROGMEM uint8_t ssd1306xled_font6x8 [];

/** German chars for standard ASCII 6x8 font */
extern const PROGMEM uint8_t ssd1306xled_font6x8_German [];

/** Standard ASCII 6x8 AB font with only capital letters */
extern const PROGMEM uint8_t ssd1306xled_font6x8_AB [];

/** Standard ASCII 5x7 font */
extern const PROGMEM uint8_t ssd1306xled_font5x7 [];

/** Standard ASCII 5x7 font with only capital letters */
extern const PROGMEM uint8_t ssd1306xled_font5x7_AB [];

/** Digital ASCII 5x7 font with only digits and operation signs */
extern const PROGMEM uint8_t digital_font5x7_123[];

/** Digital ASCII 5x7 font with only capital letters */
extern const PROGMEM uint8_t digital_font5x7_AB[];

/** Digital ASCII 5x7 font */
extern const PROGMEM uint8_t digital_font5x7[];

/** Calibri ASCII 11x12 font */
extern const PROGMEM uint8_t free_calibri11x12[];

/** Calibri ASCII 11x12 font cyrillic */
extern const PROGMEM uint8_t free_calibri11x12_cyrillic[];

/** Calibri ASCII 11x12 font basic latin */
extern const PROGMEM uint8_t free_calibri11x12_latin[];


/**
 * Standard ASCII font 11x16 with digits only (Ascii codes 32 - 64).
 * @warning can be used only with ssd1306_printFixed() and ssd1306_setFixedFont() functions.
 */
extern const PROGMEM uint8_t courier_new_font11x16_digits [];

/**
 * Comic Sans ASCII font 24x32 with digits only (Ascii codes 32 - 64).
 * @warning can be used only with ssd1306_printFixed() and ssd1306_setFixedFont() functions.
 */
extern const PROGMEM uint8_t comic_sans_font24x32_123 [];

/**
 * @brief Sets cursor position for text mode print functions.
 *
 * Sets cursor position for text mode print functions.
 * @param x xpos in pixels
 * @param y ypos in pixels
 */
void ssd1306_setCursor(lcdint_t x, lcdint_t y);

/**
 * Function allows to set another fixed font for the library.
 * By default, the font supports only first 128 - 32 ascii chars.
 * Please refer to github wiki on how to generate new fonts.
 * @param progmemFont - font to setup located in Flash area
 */
void ssd1306_setFixedFont(const uint8_t * progmemFont);

/**
 * Function allows to set another free font for the library.
 * By default, the font supports only first 128 - 32 ascii chars.
 * Please refer to github wiki on how to generate new fonts.
 * @param progmemFont - font to setup located in Flash area
 * @note This function supports new fonts of ssd1306 library 1.7.8 and above
 */
void ssd1306_setFreeFont(const uint8_t * progmemFont);

/**
 * Function allows sets secondary font for specific language.
 * Use it if you want to use additional font to combine capabilities of
 * ascii fonts and language specific font.
 * @param progmemUnicode font containing unicode table (refer to
 *        ssd1306xled_font6x8_German as example).
 */
void ssd1306_setSecondaryFont(const uint8_t * progmemUnicode);

/**
 * Function allows to set another font for the library.
 * By default, the font supports only first 128 - 32 ascii chars.
 * First 32 chars of ascii table are non-printable, and removed
 * from the font table to reduce flash memory consumption.
 * Default font doesn't support russian characters. Using
 * this function you can implement your own fonts.
 * First font char must be started with \<space\> image.
 * @param progmemFont - font to setup located in Flash area
 * @deprecated Use ssd1306_setFixedFont() instead.
 */
void ssd1306_setFont6x8(const uint8_t * progmemFont) __attribute__ ((deprecated));

/**
 * @brief returns char data for currently set (active) font.
 *
 * Function returns char data for currently set font: pointer to progmem
 * data for specified char, and width, height of the char. You can use these
 * data to draw char manually using ssd1306_drawBitmap(), ssd1306_drawMonoBitmap8()
 * or NanoCanvasOps::drawBitmap1().
 *
 * @param ch char to read from set font
 * @param info pointer to SCharInfo structure to fill with char data
 *
 * @see ssd1306_setFixedFont
 */
void ssd1306_getCharBitmap(uint16_t ch, SCharInfo *info);

/**
 * Enables utf8 support for all text-functions.
 * @note Unicode-16 only supported in text decoding functions.
 */
void ssd1306_enableUtf8Mode(void);

/**
 * Enables ascii mode for all text-functions. No any decoding will be performed
 */
void ssd1306_enableAsciiMode(void);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/**
 * Function allows to set and use squix font.
 * @param progmemFont - font to setup located in Flash area
 * @note This function supports squix fonts for ssd1306 library 1.7.8 and above
 * @warning Squix fonts are not fully supported, use them at your own risk
 */
void ssd1306_setSquixFont(const uint8_t * progmemFont);

/**
 * Returns 16-bit unicode char, encoded in utf8
 *         SSD1306_MORE_CHARS_REQUIRED if more characters is expected
 * @param ch character byte to decode
 * @return 16-bit unicode char, encoded in utf8
 *         SSD1306_MORE_CHARS_REQUIRED if more characters is expected
 */
uint16_t ssd1306_unicode16FromUtf8(uint8_t ch);
#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // SSD1306_FONTS_H
