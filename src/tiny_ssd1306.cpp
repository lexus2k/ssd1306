/*
    Copyright (C) 2017 Alexey Dynda
    Copyright (C) 2017 Kidelo

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
#include "intf/ssd1306_interface.h"
#include "lcd/ssd1306_commands.h"

size_t TinySSD1306::write(uint8_t ch)
{
    // skip non printable chars and prepare the line
    const char str[2] = { static_cast<char>( ch < ' ' ? '_' : ch ), '\0' };

    // end of line reached ?
    const bool eol = m_xpos > ( width() - FONT_X_SIZE );

    // special control code ?
    const bool cr  = '\r' == ch;
    const bool lf  = '\n' == ch;

    // skip print of char
    const bool skip = cr || lf;

    // carriage return requested or EOL in text ?
    if ( eol || cr )
    {
        m_xpos = 0;
    }

    // new line requested or pending line break of last call ..
    if ( eol || m_pending_lf )
    {
        m_ypos += 1;
    }

    // new line outside display requested
    if ( m_ypos >= ( height() / FONT_Y_SIZE ) )
    {
        // jump to home
        m_ypos = 0;

        // clear display
        clear();
    }

    // print it ?
    if ( !skip )
    {
        // normal processing
        ssd1306_charF6x8( m_xpos, m_ypos, str, STYLE_NORMAL );

        // forward
        m_xpos += FONT_X_SIZE;
    }

    // remember line feed before next draw
    m_pending_lf = lf;

    // always consumed, fulfill behavior of write()
    return 1;
}
