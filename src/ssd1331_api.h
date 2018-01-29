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
 * @file ssd1331_api.h SSD1331 specific draw functions
 */

#ifndef _SSD1331_H_
#define _SSD1331_H_

#include "nano_gfx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////
//                 DISPLAY CONTROL FUNCTIONS
///////////////////////////////////////////////////////////////////////

/**
 * @defgroup LCD_DISPLAY_API LCD Display control functions
 * @{
 */


/**
 * @}
 */

///////////////////////////////////////////////////////////////////////
//                 DIRECT GRAPH FUNCTIONS
///////////////////////////////////////////////////////////////////////

/**
 * @defgroup LCD_SSD1331_API SSD1331 only API functions
 * @{
 */

/**
 * Draws line
 * @param x1 - x position in pixels of start point
 * @param y1 - y position in pixels of start point
 * @param x2 - x position in pixels of end point
 * @param y2 - y position in pixels of end point
 * @param color - color of the line, refer to RGB_COLOR8 macros
 *
 * @note This API can be used only with ssd1331 RGB oled displays
 */
void         ssd1331_drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // _SSD1331_H_
