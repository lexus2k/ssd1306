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
typedef struct
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

} SPRITE;

// ----------------------------------------------------------------------------
#endif // _NANO_GFX_TYPES_H_
