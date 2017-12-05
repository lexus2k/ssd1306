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
 * @file nano_gfx_types.h Basic structures of nano gfx library
 */

#ifndef _NANO_GFX_TYPES_H_
#define _NANO_GFX_TYPES_H_

#include <stdint.h>
#include <Arduino.h>

/** Pointer type to LCD display initialization function */
typedef void (*InitFunction)();

/** Supported font styles */
typedef enum
{
    STYLE_NORMAL,
    STYLE_BOLD,
    STYLE_ITALIC,
} EFontStyle;

/**
 * Rectangle region. not used now
 */
typedef struct
{
    /// left
    uint8_t left;
    /// top
    uint8_t top;
    /// right
    uint8_t right;
    /// bottom
    uint8_t bottom;
} SSD1306_RECT;


/**
 * SPRITE structure represents logical graphics object
 */
typedef struct SPRITE
{
    /// draw position X on the screen
    uint8_t x;
    /// draw position Y on the screen
    uint8_t y;
    /// sprite width
    uint8_t w;
    /// last draw position X on the screen
    uint8_t lx;
    /// last draw position Y on the screen
    uint8_t ly;
    /// Pointer to PROGMEM data, representing sprite image
    const uint8_t * data;
    /// Pointer to PROGMEM data, representing sprite transparencyMask (can be nullptr)
    const uint8_t * transparentMask;

    /**
     * Updates active position of the sprite (doesn't redraw it)
     * @param x - horizontal position
     * @param y - vertical position
     */
    void setPos(uint8_t x, uint8_t y);

    /**
     * Draws sprite on the display. Position can be changed by
     * updating x and y fields of SPRITE structure.
     */
    void draw();

    /**
     * Clears some sprite parts in old position on the display.
     */
    void eraseTrace();

    /**
     * Clears sprite from the display leaving black rectangle.
     */
    void erase();

    /**
     * Returns true if sprite is moved not far from previous position,
     * and old and new rects have intersection.
     */
    inline bool isNearMove() const
    {
        /* We emulate abs function for unsigned vars here */
        return (((uint8_t)(x-lx)<w) || ((uint8_t)(lx-x)<w)) &&
               (((uint8_t)(y-ly)<8) || ((uint8_t)(ly-y)<8));
    };
                                                       
    /**
     * Returns area in 8-pixel blocks, which is used by the sprite.
     *
     * For example, if sprite pixels coordinates are 10,18 and size is 8x8,
     * the rect will be (left:1,top:2,right:2,bottom:3).
     * if sprite pixels coordinates are 32,16 and size is 8x8,
     * the rect will be (left:4,top:2,right:4,bottom:2).
     */
    inline SSD1306_RECT getRect() const
    {
        uint8_t right = ((x + w - 1)>>3);
        uint8_t bottom = ((y + 7)>>3);
        uint8_t left = x>>3; left = left < right ? left: 0;
        uint8_t top = y>>3; top = top < bottom ? top: 0;
        return (SSD1306_RECT){ left, top, right, bottom };
    };

    /**
     * Returns area in 8-pixel blocks, which was used by the sprite last time
     * For example, if sprite pixels coordinates are 10,18 and size is 8x8,
     * the rect will be (left:1,top:2,right:2,bottom:3).
     * if sprite pixels coordinates are 32,16 and size is 8x8,
     * the rect will be (left:4,top:2,right:4,bottom:2).
     */
    inline SSD1306_RECT getLRect() const
    {
        uint8_t left = lx;
        uint8_t top = ly;
        uint8_t right = (uint8_t)(lx + w - 1);
        uint8_t bottom = (uint8_t)(ly + 7);
        left = left < right ? left: 0;
        top = top < bottom ? top: 0;
        return (SSD1306_RECT){ left, top, right, bottom };
    };

    /**
     * Returns area in 8-pixel blocks, which includes old and new position
     * For example, if sprite pixels coordinates are 12,18 and size is 8x8,
     * and sprite is moved to the right by 6 pixels, the rect will be
     * (left:1,top:2,right:3,bottom:3).
     */
    inline SSD1306_RECT getUpdateRect() const
    {
        uint8_t left = min(x,lx);
        uint8_t top = min(y,ly);
        uint8_t right = max((uint8_t)(x + w - 1), (uint8_t)(lx + w - 1));
        if (((uint8_t)(lx + w - 1) < w) && (right > 2*w))
        {
            right = (uint8_t)(lx + w - 1);
        }
        uint8_t bottom = max((uint8_t)(y + 7), (uint8_t)(ly + 7));
        if (((uint8_t)(ly + 7) < 8) && (bottom > 16))
        {
            bottom = (uint8_t)(ly + 7);
        }
        if ( left > right ) left = 0;
        if ( top > bottom ) top = 0;
        return (SSD1306_RECT){ left, top, right, bottom };
    };
} SPRITE;

// ----------------------------------------------------------------------------
#endif // _NANO_GFX_TYPES_H_
