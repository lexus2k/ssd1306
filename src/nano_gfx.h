/*
    MIT License

    Copyright (c) 2016-2018, Alexey Dynda

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
 * @file nano_gfx.h Drawing in memory buffer
 */


#ifndef _NANO_GFX_H_
#define _NANO_GFX_H_

#include "nano_gfx_types.h"
#include "font6x8.h"

/**
 * NanoCanvas represents objects for drawing in memory buffer
 */
class NanoCanvas
{
public:
    /**
     * Creates new canvas object.
     * Width can be of any value.
     * Height should be divided by 8.
     * Memory buffer must be not less than w * h / 8.
     *
     * @param w - width
     * @param h - height
     * @param bytes - pointer to memory buffer to use
     */
    NanoCanvas(uint8_t w, uint8_t h, uint8_t *bytes)
    {
        m_w = w;
        m_h = h;
        m_p = 3;
        m_invertByte = 0;
        while (w >> (m_p+1)) { m_p++; };
        m_bytes = bytes;
        clear();
    };

    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     */
    inline void drawPixel(uint8_t x, uint8_t y) __attribute__ ((deprecated)) { putPixel(x,y); };

    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     */
    void putPixel(uint8_t x, uint8_t y);

    /**
     * Draws rectangle
     * @param x1 - left boundary
     * @param y1 - top boundary
     * @param x2 - right boundary
     * @param y2 - bottom boundary
     */
    void drawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

    /**
     * Draws horizontal line
     * @param x1 - left boundary
     * @param y1 - position Y
     * @param x2 - right boundary
     */
    void drawHLine(uint8_t x1, uint8_t y1, uint8_t x2);

    /**
     * Draws vertical line
     * @param x1 - position X
     * @param y1 - top boundary
     * @param y2 - bottom boundary
     */
    void drawVLine(uint8_t x1, uint8_t y1, uint8_t y2);

    /**
     * Draws filled rectangle
     * @param x1 - left boundary
     * @param y1 - top boundary
     * @param x2 - right boundary
     * @param y2 - bottom boundary
     * @param templ - template to use for filling rectangle
     */
    void fillRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t templ);

    /**
     * Clears canvas
     */
    void clear();

    /**
     * Prints text to canvas buffer
     * @param x - start position X
     * @param y - start position Y
     * @param ch - text to print (null-terminated)
     * @param style - font style (EFontStyle), normal by default
     */
    inline void char_f6x8(uint8_t x, uint8_t y, const char ch[], EFontStyle style = STYLE_NORMAL)
    __attribute__ ((deprecated))
    { charF6x8(x,y,ch,style); };

    /**
     * Prints text to canvas buffer
     * @param x - start position X
     * @param y - start position Y
     * @param ch - text to print (null-terminated)
     * @param style - font style (EFontStyle), normal by default
     */
    void charF6x8(uint8_t x, uint8_t y, const char ch[], EFontStyle style = STYLE_NORMAL);

    /**
     * Prints text to canvas buffer using double size font 12x16
     * @param x - start position X
     * @param y - start position Y
     * @param ch - text to print (null-terminated)
     * @param style - font style (EFontStyle), normal by default
     */
    void charF12x16(uint8_t x, uint8_t y, const char ch[], EFontStyle style = STYLE_NORMAL);

    /**
     * Prints text to canvas buffer using fixed font.
     * @param xpos - start position X
     * @param y - start position Y
     * @param ch - text to print (null-terminated)
     * @param style - font style (EFontStyle), normal by default
     * @see ssd1306_setFixedFont().
     */
    void printFixed(uint8_t xpos, uint8_t y, const char ch[], EFontStyle style = STYLE_NORMAL);

    /**
     * Prints text to canvas buffer using fixed font double size.
     * @param xpos - start position X
     * @param y - start position Y
     * @param ch - text to print (null-terminated)
     * @param style - font style (EFontStyle), normal by default
     * @see ssd1306_setFixedFont().
     */
    void printFixed2x(uint8_t xpos, uint8_t y, const char ch[], EFontStyle style = STYLE_NORMAL);

    /**
     * Sprite is small image 8x8, sprite doesn't change background
     * Reads sprite from Flash memory
     */
    void drawSpritePgm(uint8_t x, uint8_t y, const uint8_t sprite[]);

    /**
     * Draw bitmap to the buffer from SRAM.
     * @param x - horizontal position in pixels
     * @param y - vertical position in pixels
     * @param w - width of bitmap in pixels
     * @param h - height of bitmap in pixels (must be divided by 8)
     * @param buf - pointer to data, located in Flash: each byte represents 8 vertical pixels.
     */
    void drawBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf);

    /**
     * Sprite is small image 8x8, sprite doesn't change background
     * Reads sprite from SRAM memory
     */
    void drawSprite(uint8_t x, uint8_t y, const uint8_t sprite[]);

    /**
     * Draws sprite in the buffer
     * @param sprite - pointer to SPRITE structure containing sprite information
     */
    void drawSprite(SPRITE *sprite);

    /**
     * Returns canvas width
     */
    inline uint8_t width() const { return m_w; };

    /**
     * Returns canvas height
     */
    inline uint8_t height() const { return m_h; };

    /**
     * Returns canvas buffer
     */
    inline uint8_t *buffer() const { return m_bytes; };

    /**
     * Inverts content in the buffer.
     * white becomes black and wise versa.
     */
    void invert();

    /**
     * Flips image horizontally.
     */
    void flipH();

   /**
    * Draws canvas on the LCD display
    * @param x - horizontal position in pixels
    * @param y - vertical position in blocks (pixels/8)
    */
    void blt(uint8_t x, uint8_t y);

private:
    uint8_t m_w;
    uint8_t m_p;
    uint8_t m_h;
    uint8_t m_invertByte;
    uint8_t *m_bytes;
};

#endif

