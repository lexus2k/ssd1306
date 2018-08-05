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

/** Standard ASCII 6x8 font */
extern const PROGMEM uint8_t ssd1306xled_font6x8 [];

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
void       ssd1306_setCursor(lcdint_t x, lcdint_t y);

/**
 * Function allows to set another fixed font for the library.
 * By default, the font supports only first 128 - 32 ascii chars.
 * First 32 chars of ascii table are non-printable, and removed
 * from the font table to reduce flash memory consumption.
 * Default font doesn't support russian characters. Using
 * this function you can implement your own fonts.
 * First font char must be started with \<space\> image.
 * Font data should be in the following format:
 * | 0x00 | 0xWW | 0xHH | 0xAA | FONT DATA |,
 * where 0xWW - width in pixels, 0xHH - height in pixels, 0xAA - ascii offset (0x20).<br>
 * For fixed font 6x8 each char is presented by 6 bytes:<br>
 * COL0, COL1, COL2, COL3, COL4, COL5.<br>
 * For fixed font 4x16 each char is presented by 4x16/8 = 8 bytes:<br>
 * ROW0: COL0, COL1, COL2, COL3,<br>
 * ROW1: COL0, COL1, COL2, COL3<br>
 * @param progmemFont - font to setup located in Flash area
 */
void ssd1306_setFixedFont(const uint8_t * progmemFont);

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
 * data to draw char manually using ssd1306_drawBitmap(), ssd1331_drawMonoBitmap8()
 * or NanoCanvasOps::drawBitmap1().
 *
 * @param ch char to read from set font
 * @param info pointer to SCharInfo structure to fill with char data
 *
 * @see ssd1306_setFixedFont
 */
void ssd1306_getCharBitmap(char ch, SCharInfo *info);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
const uint8_t *ssd1306_getCharGlyph(char ch);

const uint8_t *ssd1306_getU16CharGlyph(uint16_t unicode);
#endif

/**
 * Enables utf8 support for all text-functions.
 * @note Unicode-16 only supported in text decoding functions.
 */
void ssd1306_enableUtf8Mode();

/**
 * Enables ascii mode for all text-functions. No any decoding will be performed
 */
void ssd1306_enableAsciiMode();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/**
 * Returns 16-bit unicode char, encoded in utf8
 *         0xffff if more characters is expected
 * @param ch character byte to decode
 * @return 16-bit unicode char, encoded in utf8
 *         0xffff if more characters is expected
 */
uint16_t get_unicode16_from_utf8(uint8_t ch);
#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // SSD1306_FONTS_H
