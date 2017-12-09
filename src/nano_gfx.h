/*
    Copyright (C) 2016-2017 Alexey Dynda

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
     * Prints text to canvas buffer
     * @param x - start position X
     * @param y - start position Y
     * @param ch - text to print (null-terminated)
     * @param style - font style (EFontStyle), normal by default
     */
    void charF12x16(uint8_t x, uint8_t y, const char ch[], EFontStyle style = STYLE_NORMAL);

    /**
     * Sprite is small image 8x8, sprite doesn't change background
     * Reads sprite from Flash memory
     */
    void drawSpritePgm(uint8_t x, uint8_t y, const uint8_t sprite[]);

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

