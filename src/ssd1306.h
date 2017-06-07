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

#include "ssd1306_i2c.h"

#ifndef SSD1306_SA
    /* Write command will be SSD1306_SA<<1 and read will be SSD1306_SA<<1 | 1 */
    #define SSD1306_SA    0x3C  // Slave address
#endif

#define SSD1306_DEFAULT_ADDRESS 0x78
#define SSD1306_SETCONTRAST    0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON   0xA5
#define SSD1306_NORMALDISPLAY  0xA6
#define SSD1306_INVERTDISPLAY  0xA7
#define SSD1306_DISPLAYOFF     0xAE
#define SSD1306_DISPLAYON      0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS     0xDA
#define SSD1306_SETVCOMDETECT  0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE   0xD9
#define SSD1306_SETMULTIPLEX   0xA8
#define SSD1306_SETLOWCOLUMN   0x00
#define SSD1306_SETHIGHCOLUMN  0x10
#define SSD1306_SETSTARTLINE   0x40
#define SSD1306_MEMORYMODE     0x20
#define SSD1306_COLUMNADDR     0x21
#define SSD1306_PAGEADDR       0x22
#define SSD1306_COMSCANINC     0xC0
#define SSD1306_COMSCANDEC     0xC8
#define SSD1306_SEGREMAP       0xA0
#define SSD1306_CHARGEPUMP     0x8D
#define SSD1306_SWITCHCAPVCC   0x2
#define SSD1306_NOP            0xE3

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
} SPRITE;

/**
 * Init display
 */
void         ssd1306_init();

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

SSD1306_RECT ssd1306_rect(SPRITE * sprite);

SSD1306_RECT ssd1306_rectFromSprite(uint8_t x, uint8_t y, uint8_t w);

// ----------------------------------------------------------------------------
#endif // _SSD1306_H_
