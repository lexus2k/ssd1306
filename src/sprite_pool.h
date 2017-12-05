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

    /// Defines max sprites number supported by SpritePool
    static const uint8_t MAX_SPRITES = 10;

    /**
     * Creates empty SpritePool object.
     * It is able to hold up to 8 sprites.
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

