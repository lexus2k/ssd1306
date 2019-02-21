/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

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

#ifndef _NANO_OBJECT_H_
#define _NANO_OBJECT_H_

#include "point.h"
#include "rect.h"
#include "ssd1306_hal/io.h"
#include "tiler.h"

/**
 * @ingroup NANO_ENGINE_API
 * @{
 */

template<class T>
class NanoObjectList;

/**
 * This is base class for all NanoObjects.
 */
template<class T>
class NanoObject: public NanoEngineObject<T>
{
public:
    template<class N> friend class NanoObjectList;
    /**
     * Creates basic object.
     */
    NanoObject(const NanoPoint &pos)
        : m_rect{pos, pos}
    {
    }

    NanoObject(const NanoPoint &pos, const NanoPoint &size)
        : m_rect{pos, pos + size - (NanoPoint){1,1}}
    {
    }

    /**
     * Draws nano object Engine canvas
     */
    void draw() override {};

    /**
     * Marks nano object location for refreshing on the new frame
     */
    void refresh() override
    {
        if (this->getTiler())
        {
             this->getTiler()->refreshWorld( m_rect );
        }
    }

    void update() override {}

    lcdint_t width()
    {
        return m_rect.width();
    }

    lcdint_t height()
    {
        return m_rect.height();
    }

    /**
     * Moves sprite to new position
     */
    void moveTo(const NanoPoint &p)
    {
        refresh();
        setPos( p );
        refresh();
    }

    /**
     * Moves sprite to new position by specified offset
     */
    void moveBy(const NanoPoint &p)
    {
        refresh();
        setPos( m_rect.p1 + p );
        refresh();
    }

    void resize(const NanoPoint &size)
    {
        refresh();
        setSize( size );
        refresh();
    }

    void setSize(const NanoPoint &size)
    {
        m_rect.p2.x = m_rect.p1.x + size.x - 1;
        m_rect.p2.y = m_rect.p1.y + size.y - 1;
    }

    /**
     * Sets position of NanoObject, doesn't mark for update content on the screen
     */
    void setPos(const NanoPoint &p)
    {
        m_rect = (NanoRect){ p,
                   (NanoPoint){ (lcdint_t)(p.x + m_rect.p2.x - m_rect.p1.x),
                                (lcdint_t)(p.y + m_rect.p2.y - m_rect.p1.y) } };
    }

    /**
     * Returns bottom-center point of the sprite
     */
    const NanoPoint bottom() const
    {
        return { (lcdint_t)((m_rect.p1.x + m_rect.p2.x) >> 1), m_rect.p2.y  };
    }

    /**
     * Returns top-center point of the sprite
     */
    const NanoPoint top() const
    {
        return { (lcdint_t)((m_rect.p1.x + m_rect.p2.x) >> 1), m_rect.p1.y  };
    }

    /**
     * Returns left-center point of the sprite
     */
    const NanoPoint left() const
    {
        return { m_rect.p1.x, (lcdint_t)((m_rect.p1.y + m_rect.p2.y) >> 1)  };
    }

    /**
     * Returns right-center point of the sprite
     */
    const NanoPoint right() const
    {
        return { m_rect.p2.x, (lcdint_t)((m_rect.p1.y + m_rect.p2.y) >> 1)  };
    }

    /**
     * Returns center point of the sprite
     */
    const NanoPoint center() const
    {
        return { (lcdint_t)((m_rect.p1.x + m_rect.p2.x) >> 1),
                 (lcdint_t)((m_rect.p1.y + m_rect.p2.y) >> 1) };
    }

    /**
     * Returns sprite x position
     */
    lcdint_t x( ) const { return m_rect.p1.x; }

    /**
     * Returns sprite y position
     */
    lcdint_t y( ) const { return m_rect.p1.y; }

    /**
     * Returns current sprite position (top-left corner)
     */
    const NanoPoint & getPosition() const { return m_rect.p1; }

    const NanoRect & getRect() const { return m_rect; }

protected:
    NanoRect       m_rect;
};

template<class T>
class NanoObjectList: public NanoObject<T>
{
public:
    using NanoObject<T>::NanoObject;

    NanoObject<T> *getNext(NanoObject<T> *prev = nullptr)
    {
        return static_cast<NanoObject<T>*>(prev ? prev->m_next : m_first);
    }

    NanoObject<T> *getPrev(NanoObject<T> *curr = nullptr)
    {
        NanoObject<T> *p = m_first;
        while (p)
        {
            if (p->m_next == curr)
            {
                break;
            }
            p = static_cast<NanoObject<T>*>(p->m_next);
        }
        return p;
    }

    void update() override
    {
        NanoObject<T> *p = getNext();
        while (p)
        {
            p->update();
            p = getNext( p );
        }
    }

    void refresh() override
    {
        NanoObject<T> *p = getNext();
        while (p)
        {
            p->setTiler( this->m_tiler );
            p->refresh();
            p = getNext( p );
        }
    }

    void draw() override
    {
        NanoObject<T> *p = getNext();
        while (p)
        {
            p->draw();
            p = getNext(p);
        }
    }

    bool has(NanoObject<T> &object)
    {
        NanoObject<T> *p = getNext();
        while (p && p != &object)
        {
            p = getNext(p);
        }
        return p != nullptr;
    }

    void add(NanoObject<T> &object)
    {
        if ( has( object ) )
        {
            return;
        }
        object.m_next = nullptr;
        object.setTiler( this->m_tiler );
        if ( !m_first )
        {
            m_first = &object;
        }
        else
        {
            getPrev()->m_next = &object;
        }
        object.refresh();
    }

    void insert(NanoObject<T> &object)
    {
        if ( has( object ) )
        {
            return;
        }
        object.m_next = m_first;
        object.m_tiler = this->m_tiler;
        m_first = &object;
        object.refresh();
    }

    void remove(NanoObject<T> &object)
    {
        if ( m_first == nullptr )
        {
        }
        else if ( &object == m_first )
        {
            object.refresh();
            m_first = object.m_next;
            object.m_next = nullptr;
            object.m_tiler = nullptr;
        }
        else
        {
            NanoObject<T> *p = m_first;
            while ( p->m_next )
            {
                if ( p->m_next == &object )
                {
                    object.refresh();
                    p->m_next = object.m_next;
                    object.m_next = nullptr;
                    object.m_tiler = nullptr;
                    break;
                }
                p = p->m_next;
            }
        }
    }

private:
    NanoObject<T> *m_first = nullptr;
};

/**
 * @}
 */

#endif

