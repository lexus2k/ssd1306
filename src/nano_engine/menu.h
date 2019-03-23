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

#include "canvas/point.h"
#include "canvas/rect.h"
#include "object.h"
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
     *
     * @param pos position of the sprite in global coordinates
     */
    NanoMenu(const NanoPoint &pos )
         : NanoObjectList<T>( pos )
    {
    }

    /**
     * Creates instance of NanoMenu at 0,0 position
     */
    NanoMenu(): NanoObjectList<T>( {0,0} )
    {
    }

    /**
     * Adds new menu item to the end of the list
     *
     * @param item menu item to add
     */
    void add( NanoObject<T> &item )
    {
        NanoObjectList<T>::add( item );
        item.update(); // update item to init all params
        updateMenuItemsPosition();
        if ( !m_selected )
        {
            m_selected = &item;
            item.focus();
        }
    }

    /**
     * Inserts new menu item to the beginning of the list
     *
     * @param item menu item to insert
     */
    void insert( NanoObject<T> &item )
    {
        NanoObjectList<T>::insert( item );
        item.update(); // update item to init all params
        updateMenuItemsPosition();
        if ( !m_selected )
        {
            m_selected = &item;
            item.focus();
        }
    }

    /**
     * Returns pointer to active menu item.
     * If nothing is selected returns nullptr.
     */
    NanoObject<T> *getSelected()
    {
        return m_selected;
    }

    /**
     * Moves active menu item position to the next item.
     * If active menu item is the last one, then first
     * menu item becomes active.
     */
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

    /**
     * Moves active menu item position to the previous item.
     * If active menu item is the first one, then last
     * menu item becomes active.
     */
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
    /**
     * Recalculates position of all menu items by aligning
     * each item in accordance with menu type.
     */
    virtual void updateMenuItemsPosition() = 0;

private:
    NanoObject<T> *m_selected = nullptr;
};

/**
 * Class implements menu, organized as the list.
 * Each item may have different width
 */
template<class T>
class NanoListMenu: public NanoMenu<T>
{
public:
    using NanoMenu<T>::NanoMenu;

    /**
     * Draw all menu items in NanoEngine buffer
     */
    void draw() override
    {
        this->getTiler()->getCanvas().setColor( 0xFFFF );
        this->getTiler()->getCanvas().drawRect( { this->m_rect.p1 + (NanoPoint){2, 2},
                                                this->m_rect.p2 - (NanoPoint){2, 2} } );
        NanoMenu<T>::draw();
    }

private:
    void updateMenuItemsPosition() override
    {
        NanoObject<T> *p = this->getNext();
        lcdint_t y_pos = this->m_rect.p1.y + 4;
        while (p)
        {
            p->setPos( { (lcdint_t)(this->m_rect.p1.x + 4), y_pos } );
            y_pos += p->height() + 1;
            p = this->getNext( p );
        }
        this->m_rect.p2.y = y_pos + 7;
        this->m_rect.p2.x = this->getDisplay().width();
    }
};

/**
 * Class implements menu, organized as the list.
 * Each item has the same some fixed width common
 * for the whole menu.
 */
template<class T>
class NanoFixedWidthMenu: public NanoMenu<T>
{
public:
    /**
     * Creates instance of NanoFixedWidthMenu.
     *
     * @param pos top left position of menu
     * @param size width and height of the menu
     */
    NanoFixedWidthMenu(const NanoPoint &pos, const NanoPoint &size)
        : NanoMenu<T>( pos )
    {
        this->setSize( size );
    }

    /**
     * Draw all menu items in NanoEngine buffer
     */
    void draw() override
    {
        this->getTiler()->getCanvas().setColor( 0xFFFF );
        this->getTiler()->getCanvas().drawRect( { this->m_rect.p1 + (NanoPoint){2, 2},
                                                this->m_rect.p2 - (NanoPoint){2, 2} } );
        NanoMenu<T>::draw();
    }

private:
    void updateMenuItemsPosition() override
    {
        NanoObject<T> *p = this->getNext();
        lcdint_t y_pos = this->m_rect.p1.y + 4;
        while (p)
        {
            p->setPos( { (lcdint_t)(this->m_rect.p1.x + 4), y_pos } );
            p->setSize( { (lcdint_t)(this->m_rect.width() - 8), p->height() } );
            y_pos += p->height() + 1;
            p = this->getNext( p );
        }
    }
};

/**
 * @}
 */

#endif

