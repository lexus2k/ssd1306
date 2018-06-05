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
 * This is template class for user sprites implementation.
 * It requires NanoEngine type and NanoEngine instance as arguments.
 */
template<typename T, T &E>
class NanoSprite
{
public:
    NanoSprite(const NanoPoint &pos, const NanoPoint &size, const uint8_t *bitmap)
         : m_pos(pos)
         , m_size(size)
         , m_bitmap( bitmap )
    {
    }

    void draw()
    {
        E.canvas.drawBitmap1(m_pos.x, m_pos.y, m_size.x, m_size.y, m_bitmap);
    }

    void refresh()
    {
        E.globalRefresh( m_pos.x, m_pos.y,
                         m_pos.x + m_size.x - 1, m_pos.y + m_size.y - 1 );
    }

    void moveTo(const NanoPoint &p)
    {
        refresh();
        m_pos = p;
        refresh();
    }

    void moveBy(const NanoPoint &p)
    {
        refresh();
        m_pos += p;
        refresh();
    }

    const NanoPoint bottom() const
    {
        return { m_pos.x + (m_size.x >> 1), m_pos.y + m_size.y  };
    }

    const NanoPoint top() const
    {
        return { m_pos.x + (m_size.x >> 1), m_pos.y  };
    }

    const NanoPoint left() const
    {
        return { m_pos.x, m_pos.y + (m_size.y>>1)  };
    }

    const NanoPoint right() const
    {
        return { m_pos.x + m_size.x, m_pos.y + (m_size.y>>1)  };
    }

    const NanoPoint center() const
    {
        return { m_pos.x + (m_size.x >> 1), m_pos.y + (m_size.y>>1)  };
    }

    void setBitmap( const uint8_t * bitmap )
    {
        m_bitmap = bitmap;
    }

    lcdint_t x( ) const { return m_pos.x; }
    lcdint_t y( ) const { return m_pos.y; }

protected:
    const NanoPoint   m_size;

private:
    NanoPoint         m_pos;
    const uint8_t *m_bitmap;
};

#endif

