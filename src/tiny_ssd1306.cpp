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

#include "tiny_ssd1306.h"

size_t TinySSD1306::write(uint8_t ch)
{
    const char str[2] = { static_cast<char>(ch), '\0' };
    if ( (m_xpos > width() - 6) || (ch == '\n') )
    {
        m_xpos = 0;
        m_ypos++;
    }
    ssd1306_charF6x8( m_xpos, m_ypos, str, STYLE_NORMAL );
    m_xpos += 6;
}
