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
 * @file sprite_pool.h Controlling sprites on the display
 */


#ifndef _SPRITE_POOL_H_
#define _SPRITE_POOL_H_

#include "nano_gfx.h"

/**
 * Sprites pool class automates processing of several sprites.
 * It remembers pointers to SPRITE objects, and carefully
 * updates only the areas, touched by the sprites. So, it
 * reduces number of i2c calls to SSD1306 display.
 */
class SpritePool
{
public:
    /// No free space for new sprite error
    static const uint8_t SP_ERR_NO_SPACE = 0xFF;

#if defined(ESP32) || defined(ESP8266)
    /// Defines max sprites number supported by SpritePool
    static const uint8_t MAX_SPRITES = 32;
#else
    /// Defines max sprites number supported by SpritePool
    static const uint8_t MAX_SPRITES = 10;
#endif

    /**
     * Creates empty SpritePool object.
     * It is able to hold up to 10 sprites on AVR
     * platforms and up to 32 sprites on ESP platforms.
     */
    SpritePool( );

    /**
     * Draw all areas, touched by the sprites.
     * To remove flickering, the method uses NanoCanvas
     * capabilities.
     */
    void drawSprites();

    /**
     * Redraws whole area, used by the sprites.
     */
    void refreshScreen();

    /**
     * Adds SPRITE object to the internal list of SpritePool
     * @param sprite - reference to SPRITE object
     * @return index of added object or SP_ERR_NO_SPACE in case of error.
     */
    uint8_t add( SPRITE &sprite );

    /**
     * Removes all SPRITE objects from internal list of SpritePool.
     */
    void clear();

    /**
     * Removes specific SPRITE object from the SpritePool.
     */
    void remove( SPRITE &sprite );

    /**
     * Sets active paint area region in blocks (pixels / 8)
     * @param rect - region in blocks (pixels / 8)
     */
    void setRect(SSD1306_RECT rect) { m_rect = rect; };

protected:
    /// Canvas used to draw sprites to avoid flickering.
    NanoCanvas    m_canvas;

    /// Rectangle, which specifies part of the display, used by the sprites
    SSD1306_RECT  m_rect;

    /**
     * This method is call every time 8x8 pixels block is needed to be drawn.
     * Use m_canvas field variable, which represents block to update.
     * For example, you can draw any 8x8 image, starting at 0,0 position in m_canvas.
     *
     * @param blockColumn - column of the block to redraw
     * @param blockRow    - row of the block to redraw
     * @note 128x64 display has 16 columns and 8 rows.
     */
    virtual void drawBlock(uint8_t blockColumn, uint8_t blockRow);

private:

    /// Internal buffer for Canvas
    uint8_t m_canvasBuf[8*8/8];

    /// Sprites container
    SPRITE *m_sprites[MAX_SPRITES];

    /// Count of registered sprites
    uint8_t m_count;

    void updateRegion(SSD1306_RECT ur);
};

#endif

