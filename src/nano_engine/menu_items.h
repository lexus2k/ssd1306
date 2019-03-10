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
 * @file menu_items.h Menu items classes
 */

#ifndef _NANO_MENU_ITEMS_H_
#define _NANO_MENU_ITEMS_H_

#include "menu.h"
#include "nano_gfx_types.h"
#include "ssd1306_generic.h"

/**
 * @ingroup NANO_ENGINE_API
 * @{
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

template<class T>
class NanoTestMenuItem: public NanoMenuItem<T>
{
public:
    NanoTestMenuItem()
       : NanoMenuItem<T>( {0, 0}, {48, 8} )
    {
    }

    void draw() override
    {
        if ( this->isFocused() )
        {
            this->getTiler()->getCanvas().setColor( 0xFFFF );
            this->getTiler()->getCanvas().fillRect( this->m_rect );
        }
        else
        {
            this->getTiler()->getCanvas().setColor( 0 );
            this->getTiler()->getCanvas().fillRect( this->m_rect );
            this->getTiler()->getCanvas().setColor( 0xFFFF );
            this->getTiler()->getCanvas().drawRect( this->m_rect );
        }
    }
};

#endif

/**
 * Template class for font menu item with user-defined font
 */
template<class T>
class NanoTextMenuItem: public NanoMenuItem<T>
{
public:
    /**
     * Creates instance of test menu item
     *
     * @param name text of the item to display
     */
    NanoTextMenuItem(const char *name)
       : NanoMenuItem<T>( {0, 0} )
       , m_name( name )
    {
    }

    /**
     * Updates menu item state. Automatically resizes menu item if width is
     * not defined yet
     */
    void update() override
    {
        if ( this->m_rect.width() <= 1 )
        {
            lcduint_t height;
            lcduint_t width = ssd1306_getTextSize(m_name, &height);
            this->setSize( {width, height} );
        }
    }

    /**
     * Draws text menu item in the NanoEngine buffer
     */
    void draw() override
    {
        if ( this->isFocused() )
        {
            this->getTiler()->getCanvas().setMode( CANVAS_MODE_TRANSPARENT );
            this->getTiler()->getCanvas().setColor( 0xFFFF );
            this->getTiler()->getCanvas().fillRect( this->m_rect );
            this->getTiler()->getCanvas().setColor( 0x0000 );
            this->getTiler()->getCanvas().printFixed( this->m_rect.p1.x, this->m_rect.p1.y, m_name );
        }
        else
        {
            this->getTiler()->getCanvas().setMode( CANVAS_MODE_BASIC );
            this->getTiler()->getCanvas().setColor( 0xFFFF );
            this->getTiler()->getCanvas().printFixed( this->m_rect.p1.x, this->m_rect.p1.y, m_name );
        }
    }

protected:
    /** Menu item text */
    const char *m_name;

};


/**
 * @}
 */

#endif

