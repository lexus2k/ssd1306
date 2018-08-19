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
 * @file ssd1306_16bit.h 16-bit specific draw functions
 */

#ifndef _SSD1306_16BIT_H_
#define _SSD1306_16BIT_H_

#include "nano_gfx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////
//                 DIRECT GRAPH FUNCTIONS
///////////////////////////////////////////////////////////////////////

/**
 * @defgroup LCD_16BIT_GRAPHICS DIRECT DRAW: 16-bit API functions only for color displays
 * @{
 *
 * @brief LCD direct draw functions only for color display.
 *
 * @details LCD direct draw functions are applicable for color display types. These functions will NOT work
 *        in ssd1306 compatible mode. Use ssd1306_setMode() function to change display mode to NORMAL.
 *        You can combine combine NanoEngine capabilities with these functions.
 *        Direct draw functions draw directly in GDRAM and do not use any double-buffering.
 */

/**
 * Draws 16-bit bitmap, located in SRAM, on the display
 * Each byte represents separate pixel: refer to RGB_COLOR16 to understand RGB scheme, being used.
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels
 * @param data - pointer to data, located in SRAM.
 */
void ssd1306_drawBufferFast16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *data);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
static inline void ssd1331_drawBufferFast16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *data)
{
    ssd1306_drawBufferFast16(x, y, w, h, data);
}
#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // _SSD1306_16BIT_H_
