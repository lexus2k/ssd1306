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
            this->getTiler()->get_canvas().setColor( 0xFFFF );
            this->getTiler()->get_canvas().fillRect( this->m_rect );
        }
        else
        {
            this->getTiler()->get_canvas().setColor( 0 );
            this->getTiler()->get_canvas().fillRect( this->m_rect );
            this->getTiler()->get_canvas().setColor( 0xFFFF );
            this->getTiler()->get_canvas().drawRect( this->m_rect );
        }
    }
};

/**
 * Template class for fixed font menu item
 */
template<class T>
class NanoTextMenuItem: public NanoMenuItem<T>
{
public:
    NanoTextMenuItem(const char *name)
       : NanoMenuItem<T>( {0, 0} )
       , m_name( name )
    {
    }

    void calcTextSize()
    {
        lcduint_t height;
        lcduint_t width = ssd1306_getTextSize(m_name, &height);
        this->setSize( {width, height} );
    }

    void draw() override
    {
        if ( this->isFocused() )
        {
            this->getTiler()->get_canvas().setMode( CANVAS_MODE_TRANSPARENT );
            this->getTiler()->get_canvas().setColor( 0xFFFF );
            this->getTiler()->get_canvas().fillRect( this->m_rect );
            this->getTiler()->get_canvas().setColor( 0x0000 );
            this->getTiler()->get_canvas().printFixed( this->m_rect.p1.x, this->m_rect.p1.y, m_name );
        }
        else
        {
            this->getTiler()->get_canvas().setMode( CANVAS_MODE_BASIC );
            this->getTiler()->get_canvas().setColor( 0xFFFF );
            this->getTiler()->get_canvas().printFixed( this->m_rect.p1.x, this->m_rect.p1.y, m_name );
        }
    }

protected:
    const char *m_name;
};


/**
 * @}
 */

#endif

