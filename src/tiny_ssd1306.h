/*
    Copyright (C) 2017 Alexey Dynda

    This file is part of SSD1306 library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @file tiny_ssd1306.h Drawing in memory buffer
 */


#ifndef _TINY_SSD1306_H_
#define _TINY_SSD1306_H_

#include "ssd1306.h"
#include "i2c/ssd1306_i2c.h"
#include "spi/ssd1306_spi.h"
#include <stdint.h>
#include "Print.h"


/** Initialization callback for SSD1306 128x64 OLED display */
#define SSD1306_128x64 &ssd1306_128x64_init
/** Initialization callback for SSD1306 128x32 OLED display */
#define SSD1306_128x32 &ssd1306_128x32_init
/** Initialization callback for SH1106 128x64 OLED display */
#define SH1106_128x64 &sh1106_128x64_init

/**
 * TinySSD1306 represents object to work with LCD display.
 * Easy to use:
 * ~~~~~~~~~~~~~~~{.cpp}
 * TinySSD1306  lcd( SSD1306_128x64 );
 * void setup()
 * {
 *      lcd.beginI2C();
 *
 *      lcd.clear();
 *      lcd.charF6x8(0,0,"Hello");
 * }
 * ~~~~~~~~~~~~~~~
 */
class TinySSD1306: public Print
{
public:
    /**
     * Creates object for communicating with LCD display.
     * @param lcd - initialization callback for LCD
     * @see SSD1306_128x64
     * @see SSD1306_128x32
     */
    explicit TinySSD1306( InitFunction lcd ) { m_init = lcd; };

    /**
     * Initializes lcd display.
     * SPI or i2c interface should be initializes prior to calling this method.
     */
    void begin() { m_init(); };

    /**
     * Initializes default custom i2c interface and lcd display.
     * 
     * @param scl  - i2c clock pin. Use -1 if you don't need to change default pin number
     * @param sda  - i2c data pin. Use -1 if you don't need to change default pin number
     * @param addr - i2c address of lcd display. Use 0 to leave default
     */
    void beginI2C(int8_t scl = -1, int8_t sda = -1, uint8_t addr = 0) { ssd1306_i2cInitEx(scl, sda, addr); m_init(); };

    /**
     * @brief Initializes default custom spi interface and lcd display.
     *
     * Initializes default custom i2c interface and lcd display.
     * @param csPin - chip enable pin to LCD slave (-1 if not used)
     * @param dcPin - data/command pin to control LCD dc (required)
     */
    void beginSPI(int8_t csPin = -1, int8_t dcPin = -1) { ssd1306_spiInit(csPin, dcPin); m_init(); };

    /**
     * Turns off display
     */
    void         off() { ssd1306_displayOff(); };

    /**
     * Turns on display
     */
    void         on() { ssd1306_displayOn(); };

    /**
     * Switches display to inverse mode.
     * LCD will display 0-pixels as white, and 1-pixels as black.
     */
    void         invertMode() { ssd1306_invertMode(); };

    /**
     * Switches display to normal mode.
     */
    void         normalMode() { ssd1306_normalMode(); };

    /**
     * Returns display height in pixels
     */
    uint8_t      height() { ssd1306_displayHeight(); };

    /**
     * Returns display width in pixels
     */
    uint8_t      width() { ssd1306_displayWidth(); };

    /**
     * Sets position in terms of display for text output (Print class).
     * @param x - horizontal position in pixels
     * @param y - vertical position in blocks (pixels/8)
     */
    void         setCursor(uint8_t x, uint8_t y) { m_xpos = x; m_ypos = y; }

    /**
     * Fills screen with pattern byte.
     * @param fill_Data - pattern to fill display with.
     */
    void         fill(uint8_t fill_Data) { ssd1306_fillScreen(fill_Data); };

    /**
     * Fills screen with zero-byte
     */
    void         clear() { ssd1306_clearScreen(); };

    /**
     * All drawing functions start to work in negative mode.
     * Old picture on the display remains unchanged.
     */
    void         negativeMode() { ssd1306_negativeMode(); };

    /**
     * All drawing functions start to work in positive (default) mode.
     * Old picture on the display remains unchanged.
     */
    void         positiveMode() { ssd1306_positiveMode(); };

    /**
     * Prints text to screen using font 6x8.
     * @param x - horizontal position in pixels
     * @param y - vertical position in blocks (pixels/8)
     * @param ch - NULL-terminated string to print
     * @param style - font style (EFontStyle), normal by default.
     * @returns number of chars in string
     */
    uint8_t      charF6x8(uint8_t x, uint8_t y,
                          const char ch[],
                          EFontStyle style = STYLE_NORMAL )
    { ssd1306_charF6x8(x, y, ch, style); };

    /**
     * Prints text to screen using double size font 12x16.
     * @param xpos - horizontal position in pixels
     * @param y - vertical position in blocks (pixels/8)
     * @param ch - NULL-terminated string to print
     * @param style - font style (EFontStyle).
     * @returns number of chars in string
     */
    uint8_t      charF12x16(uint8_t xpos, uint8_t y,
                            const char ch[],
                            EFontStyle style = STYLE_NORMAL)
    { ssd1306_charF12x16( xpos, y, ch, style ); };


    /**
     * Prints text to screen using set font.
     * If real text ends before right boundary,
     * the remaining part on the display will be erased till right
     * boundary.
     * @param left - horizontal position in pixels
     * @param y - vertical position in blocks (pixels/8)
     * @param ch - NULL-terminated string to print
     * @param style - font style (EFontStyle), normal by default.
     * @param right - right boundary of the text to output
     * @returns number of chars in string
     */
    uint8_t      charF6x8_eol(uint8_t left, uint8_t y,
                              const char ch[],
                              EFontStyle style,
                              uint8_t right)
    { ssd1306_charF6x8_eol(left, y, ch, style, right); };

    /**
     * Put single pixel on the LCD.
     *
     * @warning Please, take into account that there is no way
     * to read data from ssd1306, thus since each byte contains
     * 8 pixels, all other pixels in the same byte will be cleared
     * on the display. Use TinySSD1306::putPixels() instead.
     * If you need to have buffered output, please, refer to NanoCanvas.
     *
     * @param x - horizontal position in pixels
     * @param y - vertical position in pixels
     */
    void putPixel(uint8_t x, uint8_t y) { ssd1306_putPixel(x,y); };

    /**
     * Puts eight vertical pixels on the LCD at once.
     *
     * ~~~~~~~~~~~~~~~{.cpp}
     * // Draw 8 vertical pixels, starting at [10,16] position 
     * lcd.putPixels(10,2,0xFF);
     * // Draw 4 vertical pixels, starting at [32,28] position
     * lcd.putPixels(32,3,0x0F);
     * ~~~~~~~~~~~~~~~
     *
     * @param x - horizontal position in pixels
     * @param y - vertical position in blocks (pixels/8)
     * @param pixels - bit-pixels to draw on display
     */
    void         putPixels(uint8_t x, uint8_t y, uint8_t pixels) { ssd1306_putPixels(x,y,pixels); };

    /**
     * Draws rectangle
     * @param x1 - left boundary in pixel units
     * @param y1 - top boundary in pixel units
     * @param x2 - right boundary in pixel units
     * @param y2 - bottom boundary int pixel units
     */
    void         drawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) { ssd1306_drawRect(x1, y1, x2, y2); };

    /**
     * Draws horizontal line
     * @param x1 - left boundary in pixels
     * @param y1 - position Y in pixels
     * @param x2 - right boundary in pixels
     */
    void         drawHLine(uint8_t x1, uint8_t y1, uint8_t x2) { ssd1306_drawHLine(x1, y1, x2); };

    /**
     * Draws vertical line
     * @param x1 - position X in pixels
     * @param y1 - top boundary in pixels
     * @param y2 - bottom boundary in pixels
     */
    void         drawVLine(uint8_t x1, uint8_t y1, uint8_t y2) { ssd1306_drawVLine(x1, y1, y2); };

    /**
     * Draws bitmap, located in SRAM, on the display
     * Each byte represents 8 vertical pixels. 
     *
     * ~~~~~~~~~~~~~~~{.c}
     * // Draw small rectangle 3x8 at position 10,8
     * uint8_t buffer[3] = { 0xFF, 0x81, 0xFF };
     * lcd.drawBuffer(10, 1, 3, 8, buffer);
     * ~~~~~~~~~~~~~~~
     *
     * @param x - horizontal position in pixels
     * @param y - vertical position in blocks (pixels/8)
     * @param w - width of bitmap in pixels
     * @param h - height of bitmap in pixels (must be divided by 8)
     * @param buf - pointer to data, located in SRAM: each byte represents 8 vertical pixels.
     */
    void         drawBuffer(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf) { ssd1306_drawBuffer(x,y,w,h,buf); };

    /**
     * Draws bitmap, located in Flash, on the display
     *
     * @param x - horizontal position in pixels
     * @param y - vertical position in blocks (pixels/8)
     * @param w - width of bitmap in pixels
     * @param h - height of bitmap in pixels (must be divided by 8)
     * @param buf - pointer to data, located in Flash: each byte represents 8 vertical pixels.
     */
    void         drawBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf) { ssd1306_drawBitmap(x,y,w,h,buf); };

    /**
     * Fills block with black pixels
     * @param x - horizontal position in pixels
     * @param y - vertical position in blocks (pixels/8)
     * @param w - width of block in pixels
     * @param h - height of block in pixels (must be divided by 8)
     * @note usually this method is used to erase bitmap on the screen.
     */
    void         clearBlock(uint8_t x, uint8_t y, uint8_t w, uint8_t h) { ssd1306_clearBlock(x,y,w,h); };

    /**
     * Writes single character to the display
     * @param ch - character to write
     */
    virtual size_t write(uint8_t ch);
private:

    /** lcd display initialization function */
    InitFunction m_init;

    uint8_t      m_xpos = 0;
    uint8_t      m_ypos = 0;
};

#endif

