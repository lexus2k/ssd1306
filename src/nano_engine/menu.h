/*
    MIT License

    Copyright (c) 2019, Alexey Dynda

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
 * @file menu.h Menu class
 */

#ifndef _NANO_MENU_H_
#define _NANO_MENU_H_

#include "point.h"
#include "object.h"
#include "rect.h"
#include "ssd1306_hal/io.h"

/**
 * @ingroup NANO_ENGINE_API
 * @{
 */

class NanoMenuItem: public NanoObject
{
public:
    NanoMenuItem(): NanoObject( {0, 0}, {48, 8} )
    {
    }

    void draw() override;

    void focus()
    {
        m_focused = true;
        refresh();
    }

    void defocus()
    {
        m_focused = false;
        refresh();
    }

private:
    bool m_focused = false;
};

/**
 * This is base class for user menu implementations.
 * NanoMenu can work only as part of NanoEngine.
 */
class NanoMenu: public NanoObjectList
{
public:
    /**
     * Creates menu object.
     * @param pos position of the sprite in global coordinates
     * @param bitmap sprite content (in flash memory)
     */
    NanoMenu(const NanoPoint &pos )
         : NanoObjectList( pos )
    {
    }

    NanoMenu(): NanoObjectList( {0,0} )
    {
    }

    void add( NanoMenuItem &item )
    {
        NanoObjectList::add( item );
        refresh();
        if ( !m_selected )
        {
            m_selected = &item;
            item.focus();
        }
    }

    void insert( NanoMenuItem &item )
    {
        NanoObjectList::insert( item );
        refresh();
        if ( !m_selected )
        {
            m_selected = &item;
            item.focus();
        }
    }

    void refresh() override
    {
        updateMenuItemsPosition();
        NanoObjectList::refresh();
    }

    NanoMenuItem *getSelected()
    {
        return m_selected;
    }

    void down()
    {
        m_selected->defocus();
        m_selected = static_cast<NanoMenuItem*>(getNext( m_selected ));
        if ( !m_selected )
        {
            m_selected = static_cast<NanoMenuItem*>(getNext());
        }
        m_selected->focus();
    }

    void up()
    {
        m_selected->defocus();
        m_selected = static_cast<NanoMenuItem*>(getPrev( m_selected ));
        if ( !m_selected )
        {
            m_selected = static_cast<NanoMenuItem*>(getPrev());
        }
        m_selected->focus();
    }

protected:
    virtual void updateMenuItemsPosition()
    {
        NanoObject *p = getNext( );
        lcdint_t y_pos = m_rect.p1.y;
        while (p)
        {
            p->setPos( { m_rect.p1.x, y_pos } );
            y_pos += p->height() + 1;
            p = getNext( p );
        }
    }

private:
    NanoMenuItem *m_selected = nullptr;
};

/**
 * @}
 */

#endif
