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
 * @file ssd1306.h SSD1306 basic draw functions
 */


#ifndef _SSD1306_H_
#define _SSD1306_H_

#include "nano_gfx_types.h"

/**
 * Init 128x64 OLED display over i2c
 */
void         ssd1306_128x64_i2c_init();

/**
 * Init 128x32 OLED display over i2c
 */
void         ssd1306_128x32_i2c_init();

/**
 * Init default display 128x64
 */
static inline void ssd1306_init() { ssd1306_128x64_i2c_init(); }

/**
 * Set position in terms of display.
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 */
void         ssd1306_setPos(uint8_t x, uint8_t y);

/**
 * Fills screen with pattern byte
 */
void         ssd1306_fillScreen(uint8_t fill_Data);

/**
 * Fills screen with zero-byte
 */
void         ssd1306_clearScreen();

/**
 * Turns off display
 */
void         ssd1306_displayOff();


/**
 * Turns on display
 */
void         ssd1306_displayOn();

/**
 * Prints text to screen.
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param ch - NULL-terminated string to print
 */
void         ssd1306_charF6x8(uint8_t x, uint8_t y, const char ch[]);

/**
 * Draws bitmap, located in SRAM, on the display
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels (must be divided by 8)
 * @param buf - pointer to data, located in SRAM: each byte represents 8 vertical pixels.
 */
void         ssd1306_drawCanvas(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf);

/**
 * Draws bitmap, located in Flash, on the display
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels (must be divided by 8)
 * @param buf - pointer to data, located in Flash: each byte represents 8 vertical pixels.
 */
void         ssd1306_drawBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf);


/**
 * Fills block with black pixels
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param w - width of block in pixels
 * @param h - height of block in pixels (must be divided by 8)
 * @note usually this method is used to erase bitmap on the screen.
 */
void         ssd1306_clearBlock(uint8_t x, uint8_t y, uint8_t w, uint8_t h);


/**
 * Draws bitmap, located in Flash, on the display. This sprite must have 8x8 size
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param sprite - pointer to data, located in Flash: each byte represents 8 vertical pixels.
 */
void         ssd1306_drawSpriteData(uint8_t x, uint8_t y, const uint8_t *sprite);

// ----------------------------------------------------------------------------
/**
 * Draws sprite on the display. Position can be changed by
 * updating x and y fields of SPRITE structure.
 * @param sprite - pointer to SPRITE structure
 */
void         ssd1306_drawSprite(SPRITE *sprite);

/**
 * Clears sprite from the display leaving black rectangle.
 * @param sprite - pointer to SPRITE structure
 */
void         ssd1306_eraseSprite(SPRITE *sprite);

/**
 * Clears some sprite parts in old position on the display.
 * @param sprite - pointer to SPRITE structure
 */
void         ssd1306_eraseTrace(SPRITE *sprite);

/**
 * Creates sprite object. Sprite height is fixed to 8 pixels
 * @param x - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param w - width of sprite in pixels
 * @param data - pointer to data, located in Flash: each byte represents 8 vertical pixels.
 * @return SPRITE structure
 */
SPRITE       ssd1306_createSprite(uint8_t x, uint8_t y, uint8_t w, const uint8_t *data);

/**
 * Replaces image of the sprite with different data. The width must be the same as
 * the width of original sprite image
 * @param sprite - pointer to SPRITE structure
 * @param data - pointer to data, located in Flash: each byte represents 8 vertical pixels.
 */
void         ssd1306_replaceSprite(SPRITE *sprite, const uint8_t *data);

/**
 * Depricated.
 */
SSD1306_RECT ssd1306_rect(SPRITE * sprite);

/**
 * Depricated.
 */
SSD1306_RECT ssd1306_rectFromSprite(uint8_t x, uint8_t y, uint8_t w);

// ----------------------------------------------------------------------------
#endif // _SSD1306_H_
