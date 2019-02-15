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
#include "menu.h"
#include "tiler.h"

void NanoMenuItem::draw()
{
    if ( isFocused() )
    {
        m_tiler->get_canvas().setColor( 0xFFFF );
        m_tiler->get_canvas().fillRect( m_rect );
    }
    else
    {
        m_tiler->get_canvas().setColor( 0 );
        m_tiler->get_canvas().fillRect( m_rect );
        m_tiler->get_canvas().setColor( 0xFFFF );
        m_tiler->get_canvas().drawRect( m_rect );
    }
}

void NanoListMenu::draw()
{
    m_tiler->get_canvas().setColor( 0xFFFF );
    m_tiler->get_canvas().drawRect( { m_rect.p1 + (NanoPoint){3, 3},
                                      m_rect.p2 - (NanoPoint){3, 3} } );
    NanoMenu::draw();
}

