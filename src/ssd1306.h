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
 * @file ssd1306.h SSD1306 basic draw functions
 */

#ifndef _SSD1306_H_
#define _SSD1306_H_

#include "nano_gfx_types.h"
#include "ssd1306_generic.h"
#include "ssd1306_1bit.h"
#include "ssd1306_8bit.h"
#include "ssd1306_16bit.h"
#include "ssd1306_fonts.h"

#include "lcd/lcd_common.h"
#include "lcd/oled_ssd1306.h"
#include "lcd/oled_ssd1325.h"
#include "lcd/oled_ssd1327.h"
#include "lcd/oled_ssd1331.h"
#include "lcd/oled_ssd1351.h"
#include "lcd/oled_sh1106.h"
#include "lcd/lcd_pcd8544.h"
#include "lcd/lcd_il9163.h"
#include "lcd/lcd_ili9341.h"
#include "lcd/composite_video.h"

#include "lcd/oled_template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup LCD_INTERFACE_API
 * Returns display height in pixels
 */
lcduint_t      ssd1306_displayHeight(void);

/**
 * @ingroup LCD_INTERFACE_API
 * Returns display width in pixels
 */
lcduint_t      ssd1306_displayWidth(void);

#ifdef __cplusplus
}
#endif

#endif // _SSD1306_H_
