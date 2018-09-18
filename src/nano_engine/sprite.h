/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

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
 * @file sprite.h Sprite class
 */

#ifndef _NANO_SPRITE_H_
#define _NANO_SPRITE_H_

#include "point.h"
#include "rect.h"
#include "ssd1306_hal/io.h"

/**
 * @ingroup NANO_ENGINE_API
 * @{
 */

/**
 * This is template class for user sprites implementations.
 * NanoSprite can work only as part of NanoEngine, it requires
 * NanoEngine type and NanoEngine instance as arguments.
 */
template<typename T, T &E>
class NanoSprite
{
public:
    /**
     * Creates sprite object of variable size. Such sprites can
     * change their size and bitmap content.
     * @param pos position of the sprite in global coordinates
     * @param size size of sprite
     * @param bitmap sprite content (in flash memory)
     */
    NanoSprite(const NanoPoint &pos, const NanoPoint &size, const uint8_t *bitmap)
         : m_rect{pos, pos + size}
         , m_bitmap( bitmap )
    {
    }

    /**
     * Draws monochrome sprite on Engine canvas
     */
    void draw()
    {
        E.canvas.drawBitmap1(m_rect.p1.x, m_rect.p1.y, m_rect.width(), m_rect.height(), m_bitmap);
    }

    /**
     * Marks sprite locate for refreshing on the new frame
     */
    void refresh()
    {
        E.refreshWorld( m_rect );
    }

    /**
     * Moves sprite to new position
     */
    void moveTo(const NanoPoint &p)
    {
        refresh();
        m_rect = { p, p + m_rect.size() };
        refresh();
    }

    /**
     * Moves sprite to new position by specified offset
     */
    void moveBy(const NanoPoint &p)
    {
        refresh();
        m_rect += p;
        refresh();
    }

    /**
     * Returns bottom-center point of the sprite
     */
    const NanoPoint bottom() const
    {
        return { (m_rect.p1.x + m_rect.p2.x) >> 1, m_rect.p2.y  };
    }

    /**
     * Returns top-center point of the sprite
     */
    const NanoPoint top() const
    {
        return { (m_rect.p1.x + m_rect.p2.x) >> 1, m_rect.p1.y  };
    }

    /**
     * Returns left-center point of the sprite
     */
    const NanoPoint left() const
    {
        return { m_rect.p1.x, (m_rect.p1.y + m_rect.p2.y) >> 1  };
    }

    /**
     * Returns right-center point of the sprite
     */
    const NanoPoint right() const
    {
        return { m_rect.p2.x, (m_rect.p1.y + m_rect.p2.y) >> 1  };
    }

    /**
     * Returns center point of the sprite
     */
    const NanoPoint center() const
    {
        return { (m_rect.p1.x + m_rect.p2.x) >> 1, (m_rect.p1.y + m_rect.p2.y) >> 1  };
    }

    /**
     * Changes sprite bitmap to new one.
     */
    void setBitmap( const uint8_t * bitmap )
    {
        m_bitmap = bitmap;
    }

    /**
     * Returns sprite x position
     */
    lcdint_t x( ) const { return m_rect.p1.x; }

    /**
     * Returns sprite y position
     */
    lcdint_t y( ) const { return m_rect.p1.y; }

private:
    NanoRect       m_rect;
    const uint8_t *m_bitmap;
};

/**
 * This is template class for user sprites implementation.
 * It requires NanoEngine type and NanoEngine instance as arguments.
 */
template<typename T, T &E>
class NanoFixedSprite
{
public:
    /**
     * Creates sprite object of fixed size. Such sprites can
     * change their bitmap content only and position.
     * @param pos position of the sprite in global coordinates
     * @param size size of sprite
     * @param bitmap sprite content (in flash memory)
     */
    NanoFixedSprite(const NanoPoint &pos, const NanoPoint &size, const uint8_t *bitmap)
         : m_pos(pos)
         , m_size(size)
         , m_bitmap( bitmap )
    {
    }

    /**
     * Draws monochrome sprite on Engine canvas
     */
    void draw()
    {
        E.canvas.drawBitmap1(m_pos.x, m_pos.y, m_size.x, m_size.y, m_bitmap);
    }

    /**
     * Marks sprite locate for refreshing on the new frame
     */
    void refresh()
    {
        E.refreshWorld( m_pos.x, m_pos.y,
                         m_pos.x + m_size.x - 1, m_pos.y + m_size.y - 1 );
    }

    /**
     * Moves sprite to new position
     */
    void moveTo(const NanoPoint &p)
    {
        refresh();
        m_pos = p;
        refresh();
    }

    /**
     * Moves sprite to new position by specified offset
     */
    void moveBy(const NanoPoint &p)
    {
        refresh();
        m_pos += p;
        refresh();
    }

    /**
     * Returns bottom-center point of the sprite
     */
    const NanoPoint bottom() const
    {
        return { m_pos.x + (m_size.x >> 1), m_pos.y + m_size.y - 1  };
    }

    /**
     * Returns top-center point of the sprite
     */
    const NanoPoint top() const
    {
        return { m_pos.x + (m_size.x >> 1), m_pos.y  };
    }

    /**
     * Returns left-center point of the sprite
     */
    const NanoPoint left() const
    {
        return { m_pos.x, m_pos.y + (m_size.y>>1)  };
    }

    /**
     * Returns right-center point of the sprite
     */
    const NanoPoint right() const
    {
        return { m_pos.x + m_size.x - 1, m_pos.y + (m_size.y>>1)  };
    }

    /**
     * Returns center point of the sprite
     */
    const NanoPoint center() const
    {
        return { m_pos.x + (m_size.x >> 1), m_pos.y + (m_size.y>>1)  };
    }

    /**
     * Changes sprite bitmap to new one.
     */
    void setBitmap( const uint8_t * bitmap )
    {
        m_bitmap = bitmap;
    }

    /**
     * Returns current sprite position (top-left corner)
     */
    const NanoPoint & getPosition() const { return m_pos; }

    /**
     * Returns sprite x position
     */
    lcdint_t x( ) const { return m_pos.x; }

    /**
     * Returns sprite y position
     */
    lcdint_t y( ) const { return m_pos.y; }

    /**
     * Returns current sprite position (top-left corner)
     */
    const NanoPoint & pos() const { return m_pos; }

protected:
    /** fixed size of sprite */
    const NanoPoint   m_size;
    /** Sprite position in global (world) coordinates */
    NanoPoint         m_pos;

private:
    const uint8_t *m_bitmap;
};

/**
 * @}
 */

#endif

