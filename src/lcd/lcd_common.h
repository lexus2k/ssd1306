/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

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
 * @file lcd_common.h global lcd settings
 */

#ifndef _LCD_COMMON_H_
#define _LCD_COMMON_H_

#include "hal/io.h"

/**
 * @defgroup LCD_INTERFACE_API LCD High level API
 * @{
 * @brief LCD high level API for setup and programming GDRAM
 *
 * @details This group contains API functions for OLED displays initialization and
 *          direct programming of GDRAM. This API can be used to create your own
 *          graphics functions.
 */

enum
{
    /** Default type of LCD display: ssd1306 oled */
    LCD_TYPE_SSD1306,
    /** Experimental type of LCD display: pcd8544 led */
    LCD_TYPE_PCD8544,
    /** Support for sh1106 OLED display */
    LCD_TYPE_SH1106,
    /** Default type of LCD display: ssd1331 oled */
    LCD_TYPE_SSD1331,
    /** User LCD type */
    LCD_TYPE_CUSTOM,
};

/** Current display height */
extern uint8_t s_displayHeight;

/** Current display width */
extern uint8_t s_displayWidth;

/** Current selected lcd display type */
extern uint8_t g_lcd_type;

/**
 * Sends byte data to SSD1306 controller memory.
 * Performs 3 operations at once: start(), send(), stop();
 * @param data - byte to send to the controller memory
 * @note At present this function is used only in Arkanoid demo.
 */
void         ssd1306_sendData(uint8_t data) __attribute__ ((deprecated));

/**
 * Sets block in RAM of lcd display controller to write data to.
 * For ssd1306 it uses horizontal addressing mode, while for
 * sh1106 the function uses page addressing mode.
 * Width can be specified as 0, thus the library will set the right
 * region of RAM block to the right column of the display.
 * @param x - column (left region)
 * @param y - page (top page of the block)
 * @param w - width of the block in pixels to control
 *
 * @warning - this function initiates session (i2c or spi) and do not close it.
 *            To close session, please, use ssd1306_intf.stop().
 */
extern void (*ssd1306_setRamBlock)(uint8_t x, uint8_t y, uint8_t w);

/**
 * Switches to the start of next RAM page for the block, specified by
 * ssd1306_setRamBlock().
 * For ssd1306 it does nothing, while for sh1106 the function moves cursor to
 * next page.
 */
extern void (*ssd1306_nextRamPage)(void);

/**
 * Sends 8 monochrome vertical pixels to OLED driver.
 * @param data - byte, representing 8 pixels.
 */
extern void (*ssd1306_sendPixels)(uint8_t data);

/**
 * Sends buffer containing 8 monochrome vertical pixels, encoded in each byte.
 * @param buffer - buffer containing monochrome pixels.
 * @param len - length of buffer in bytes.
 */
extern void (*ssd1306_sendPixelsBuffer)(const uint8_t *buffer, uint16_t len);

/**
 * @brief Sends RGB pixel encoded in 3-3-2 format to OLED driver.
 * Sends RGB pixel encoded in 3-3-2 format to OLED driver.
 * @param data - byte, representing RGB8 pixel.
 */
extern void (*ssd1306_sendPixel8)(uint8_t data);

/**
 * @}
 */


/**
 * @}
 */

#endif /* _LCD_COMMON_H_ */
