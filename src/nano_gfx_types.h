/*
    MIT License

    Copyright (c) 2017-2019, Alexey Dynda

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
 * @file nano_gfx_types.h Basic structures of nano gfx library
 */

#ifndef _NANO_GFX_TYPES_H_
#define _NANO_GFX_TYPES_H_

#include "ssd1306_hal/io.h"

#ifndef min
/** Macros returning minimum of 2 numbers */
#define min(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef max
/** Macros returning maximum of 2 numbers */
#define max(a,b) ((a)>(b)?(a):(b))
#endif

/** Macro to generate 8-bit color for SSD1331 OLED display */
#define RGB_COLOR8(r,g,b)    ( (r & 0xE0) | ((g >> 3)&0x1C) | (b>>6) )

/** Macro to generate 16-bit color for SSD1351 OLED display */
#define RGB_COLOR16(r,g,b)   ( ((r<<8) & 0xF800) | ((g << 3)&0x07E0) | (b>>3) )

/** Macro to convert 3-3-2 color to 5-6-5 color */
#define RGB8_TO_RGB16(c)     ( (((uint16_t)c & 0b11100000) << 8) | \
                               (((uint16_t)c & 0b00011100) << 6) | \
                               (((uint16_t)c & 0b00000011) << 3) )

/** Macro to convert 8-bit RGB to 4-bit monochrome format */
#define RGB8_TO_GRAY4(rgb)  ( (rgb >> 6) + ((rgb >> 2) & 0x07) + (rgb & 0x03) )

/** Macro to generate 4-bit monochrome color from gray component */
#define GRAY_COLOR4(gray)    ( ((gray >> 4) & 0x0F) | (gray & 0xF0) )

/** Macro to convert 5-6-5 color to 3-3-2 color */
#define RGB16_TO_RGB8(c)     ( ((uint16_t)(c >> 8) & 0b11100000) | \
                               ((uint16_t)(c >> 6) & 0b00011100) | \
                               ((uint16_t)(c >> 3) & 0b00000011) )

/** Pointer type to LCD display initialization function */
typedef void (*InitFunction)(void);

/** Supported font styles */
typedef enum
{
    STYLE_NORMAL,
    STYLE_BOLD,
    STYLE_ITALIC,
} EFontStyle;

/** Supported scale font values */
typedef enum
{
    FONT_SIZE_NORMAL = 0,
    FONT_SIZE_2X     = 1,
    FONT_SIZE_4X     = 2,
    FONT_SIZE_8X     = 3,
} EFontSize;

#pragma pack(push, 1)
/** Structure describes font format in memory */
typedef struct
{
    uint8_t type;         ///< font type: 0 - Fixed Font
    uint8_t width;        ///< width in pixels
    uint8_t height;       ///< height in pixels
    uint8_t ascii_offset; ///< ascii offset
} SFontHeaderRecord;

/** Structure describes unicode block in font data */
typedef struct
{
    uint16_t start_code;  ///< unicode start code
    uint8_t count;        ///< count of unicode chars in block
} SUnicodeBlockRecord;

#pragma pack(pop)

/** Structure is used for internal font presentation */
typedef struct
{
    SFontHeaderRecord h; ///< record, containing information on font
    uint8_t count; ///< count of characters
    uint8_t pages; ///< height in pages (each page height is 8-pixels)
    uint8_t glyph_size;  ///< glyph size in bytes
    const uint8_t *primary_table; ///< font chars bits
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    const uint8_t *secondary_table; ///< font chars bits
#endif
} SFixedFontInfo;

/** Structure describes single char information */
typedef struct
{
    uint8_t width;      ///< char width in pixels
    uint8_t height;     ///< char height in pixels
    uint8_t spacing;      ///< additional spaces after char in pixels
    const uint8_t *glyph; ///< char data, located in progmem.
} SCharInfo;

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
 * @deprecated Use NanoSprite or NanoFixedSprite instead.
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

#ifdef __cplusplus
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
#endif
} SPRITE;

// ----------------------------------------------------------------------------
#endif // _NANO_GFX_TYPES_H_
