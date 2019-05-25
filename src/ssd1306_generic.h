/*
    MIT License

    Copyright (c) 2016-2019, Alexey Dynda

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
 * @file ssd1306_generic.h SSD1306 generic API functions
 */


#ifndef _SSD1306_GENERIC_H_
#define _SSD1306_GENERIC_H_

#include "nano_gfx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////
//                 GENERIC GRAPH FUNCTIONS
///////////////////////////////////////////////////////////////////////

/** Flag means that more chars are required to decode utf-8 */
#define SSD1306_MORE_CHARS_REQUIRED  0xffff

/**
 * @defgroup LCD_GENERIC_API DIRECT DRAW: Generic API functions, common for all displays and all display modes.
 * @{
 * @brief Generic API functions, common for all displays and all display modes.
 *
 * @details Generic API functions, common for all displays and all display modes.
 */

/**
 * Set position in terms of display.
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 */
void         ssd1306_setPos(uint8_t x, uint8_t y);

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void ssd1306_setFixedFont_oldStyle(const uint8_t * progmemFont);
#endif

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
 * Returns dimensions in pixels for text provided.
 * @param text text to calculate size of
 * @param height variable to store text height. Can be NULL
 * @return witdth of passed text in pixels
 */
lcduint_t ssd1306_getTextSize(const char *text, lcduint_t *height);


///////////////////////////////////////////////////////////////////////
//                 HIGH-LEVEL GRAPH FUNCTIONS
///////////////////////////////////////////////////////////////////////

/**
 * Describes menu object
 */
typedef struct
{
    /// list of menu items of the menu
    const char **items;
    /// count of menu items in the menu
    uint8_t     count;
    /// currently selected item. Internally updated.
    uint8_t     selection;
    /// selected item, when last redraw operation was performed. Internally updated.
    uint8_t     oldSelection;
    /// position of menu scrolling. Internally updated
    uint8_t     scrollPosition;
} SAppMenu;

/**
 * Creates menu object with the provided list of menu items.
 * List of menu items (strings) must exist all until menu object is no longer needed.
 * Selection is set to the first item by default.
 *
 * @param menu - Pointer to SAppMenu structure
 * @param items - array of null-termintated strings (located in SRAM)
 * @param count - count of menu items in the array
 */
void ssd1306_createMenu(SAppMenu *menu, const char **items, uint8_t count);

/**
 * Shows menu items on the display. If menu items cannot fit the display,
 * the function provides scrolling.
 *
 * @param menu - Pointer to SAppMenu structure
 *
 * @warning works only in SSD1306 compatible mode.
 */
void ssd1306_showMenu(SAppMenu *menu);

/**
 * Shows menu items on the display. If menu items cannot fit the display,
 * the function provides scrolling.
 *
 * @param menu - Pointer to SAppMenu structure
 *
 * @warning works only in 8-bit RGB normal mode.
 */
void ssd1306_showMenu8(SAppMenu *menu);

/**
 * Shows menu items on the display. If menu items cannot fit the display,
 * the function provides scrolling.
 *
 * @param menu - Pointer to SAppMenu structure
 *
 * @warning works only in 16-bit RGB normal mode.
 */
void ssd1306_showMenu16(SAppMenu *menu);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
static inline void ssd1331_showMenu8(SAppMenu *menu)
{
    ssd1306_showMenu8(menu);
}
#endif

/**
 * Updates menu items on the display. That is if selection is changed,
 * the function will update only those areas, affected by the change.
 *
 * @param menu - Pointer to SAppMenu structure
 */
void ssd1306_updateMenu(SAppMenu *menu);

/**
 * Updates menu items on the display. That is if selection is changed,
 * the function will update only those areas, affected by the change.
 *
 * @param menu - Pointer to SAppMenu structure
 *
 * @warning works only in SSD1306 compatible mode.
 */
void ssd1306_updateMenu8(SAppMenu *menu);

/**
 * Updates menu items on the display. That is if selection is changed,
 * the function will update only those areas, affected by the change.
 *
 * @param menu - Pointer to SAppMenu structure
 *
 * @warning works only in SSD1306 compatible mode.
 */
void ssd1306_updateMenu16(SAppMenu *menu);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
static inline void ssd1331_updateMenu8(SAppMenu *menu)
{
    ssd1306_updateMenu8(menu);
}
#endif

/**
 * Returns currently selected menu item.
 * First item has zero-index.
 *
 * @param menu - Pointer to SAppMenu structure
 *
 * @warning works only in 8-bit RGB normal mode.
 */
uint8_t ssd1306_menuSelection(SAppMenu *menu);

/**
 * Moves selection pointer down by 1 item. If there are no items below,
 * it will set selection pointer to the first item.
 * Use ssd1306_updateMenu() to refresh menu state on the display.
 *
 * @param menu - Pointer to SAppMenu structure
 */
void ssd1306_menuDown(SAppMenu *menu);

/**
 * Moves selection pointer up by 1 item. If selected item is the first one,
 * then selection pointer will set to the last item in menu list.
 * Use ssd1306_updateMenu() to refresh menu state on the display.
 *
 * @param menu - Pointer to SAppMenu structure
 */
void ssd1306_menuUp(SAppMenu *menu);

/**
 * Draws progress bar in the middle of the screen
 * @param progress progress value in range 0 - 100.
 */
void ssd1306_drawProgressBar(int8_t progress);

/**
 * Draws progress bar in the middle of the screen
 * @param progress progress value in range 0 - 100.
 */
void ssd1306_drawProgressBar8(int8_t progress);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // _SSD1306_GENERIC_H_
