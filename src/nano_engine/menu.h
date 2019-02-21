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
#include "lcd/lcd_common.h"

/**
 * @ingroup NANO_ENGINE_API
 * @{
 */

template<class T>
class NanoMenuItem: public NanoObject<T>
{
public:
    using NanoObject<T>::NanoObject;
};

/**
 * This is base class for user menu implementations.
 * NanoMenu can work only as part of NanoEngine.
 */
template<class T>
class NanoMenu: public NanoObjectList<T>
{
public:
    /**
     * Creates menu object.
     * @param pos position of the sprite in global coordinates
     * @param bitmap sprite content (in flash memory)
     */
    NanoMenu(const NanoPoint &pos )
         : NanoObjectList<T>( pos )
    {
    }

    NanoMenu(): NanoObjectList<T>( {0,0} )
    {
    }

    void add( NanoObject<T> &item )
    {
        NanoObjectList<T>::add( item );
        updateMenuItemsPosition();
        if ( !m_selected )
        {
            m_selected = &item;
            item.focus();
        }
    }

    void insert( NanoObject<T> &item )
    {
        NanoObjectList<T>::insert( item );
        updateMenuItemsPosition();
        if ( !m_selected )
        {
            m_selected = &item;
            item.focus();
        }
    }

    void refresh() override
    {
        NanoObjectList<T>::refresh();
    }

    NanoObject<T> *getSelected()
    {
        return m_selected;
    }

    void down()
    {
        m_selected->defocus();
        m_selected = this->getNext( m_selected );
        if ( !m_selected )
        {
            m_selected = this->getNext();
        }
        m_selected->focus();
    }

    void up()
    {
        m_selected->defocus();
        m_selected = getPrev( m_selected );
        if ( !m_selected )
        {
            m_selected = this->getPrev();
        }
        m_selected->focus();
    }

protected:
    virtual void updateMenuItemsPosition() = 0;

private:
    NanoObject<T> *m_selected = nullptr;
};

template<class T>
class NanoListMenu: public NanoMenu<T>
{
public:
    using NanoMenu<T>::NanoMenu;

    void draw() override
    {
        this->getTiler()->get_canvas().setColor( 0xFFFF );
        this->getTiler()->get_canvas().drawRect( { this->m_rect.p1 + (NanoPoint){3, 3},
                                                this->m_rect.p2 - (NanoPoint){3, 3} } );
        NanoMenu<T>::draw();
    }

private:
    void updateMenuItemsPosition() override
    {
        NanoObject<T> *p = this->getNext();
        lcdint_t y_pos = this->m_rect.p1.y + 8;
        while (p)
        {
            p->setPos( { (lcdint_t)(this->m_rect.p1.x + 8), y_pos } );
            y_pos += p->height() + 1;
            p = this->getNext( p );
        }
        this->m_rect.p2.y = y_pos + 7;
        this->m_rect.p2.x = ssd1306_lcd.width;
    }
};

/**
 * @}
 */

#endif

