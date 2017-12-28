/*
    Copyright (C) 2017 Alexey Dynda
    Copyright (C) 2017 Kidelo <kidelo@yahoo.com>

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

/**
 * @file tiny_ssd1306.cpp Drawing in memory buffer
 */

#include "tiny_ssd1306.h"
#include "intf/ssd1306_interface.h"
#include "lcd/ssd1306_commands.h"

void TinySSD1306::clear()
{
  ssd1306_clearScreen();

  // back to home
  ssd1306_sendCommand( SSD1306_SETSTARTLINE | 0 );

  // reset position
  m_xpos = m_ypos = 0;
};

size_t TinySSD1306::write(uint8_t ch)
{
    // normal height
    const uint8_t HEIGHT = ( height() / FONT_Y_SIZE );

    // skip non printable chars and prepare the line
    const char str[2] = { static_cast<char>( ch < ' ' ? '_' : ch ), '\0' };

    // end of line reached ?
    const bool eol = m_xpos > ( width() - FONT_X_SIZE );

    // special control code ?
    const bool cr  = '\r' == ch;
    const bool lf  = '\n' == ch;

    // skip print of char
    bool skip = cr || lf;

    // carriage return requested or EOL in text ?
    if ( eol || cr )
    {
        m_xpos = 0;
    }

    // new line requested or pending line break of last call ..
    if ( eol || m_pending_lf )
    {
        m_ypos += 1;

        // new line outside display requested
        if ( m_ypos >= HEIGHT )
        {
          // redraw on last line ...
          if ( T_MODE_OVERRIDE == m_mode )
          {
            // stay on position
            m_ypos = HEIGHT-1;

            // clear this line ( only some parts are overwritten by next characters )
            clearBlock( 0, m_ypos, width(), FONT_Y_SIZE );
          }

          // scrolling of text ?
          if ( T_MODE_SCROLLING == m_mode )
          {
            // second round over
            if ( m_ypos >= ( 2 * HEIGHT ) )
            {
              // reset to scrolling range
              m_ypos = HEIGHT;
            }

            // there is window of HEIGHT of text in display buffer, shift to last line that can be displayed
            const uint8_t shift_to = (( ( m_ypos - HEIGHT + 1 ) * FONT_Y_SIZE ) & ~SSD1306_SETSTARTLINE);

            // shift line ( has nothing to do with position of data in RAM )
            ssd1306_sendCommand( SSD1306_SETSTARTLINE | shift_to );

            // clear this line ( only some parts are overwritten by next characters )
            clearBlock( 0, m_ypos, width(), FONT_Y_SIZE );
          }

          // reset screen ?
          if ( T_MODE_CLEAR == m_mode )
          {
            // clear display
            clear();
          }

        } // end of if()

    } // end of if()

    // print it ?
    if ( !skip )
    {
        // use original position for write
        const uint8_t ypos = ( m_ypos < HEIGHT ) ? m_ypos : m_ypos - HEIGHT;

        // normal processing
        ssd1306_charF6x8( m_xpos, ypos, str, STYLE_NORMAL );

        // forward
        m_xpos += FONT_X_SIZE;
    }

    // remember line feed before next draw
    m_pending_lf = lf;

    // always consumed, fulfill behavior of write()
    return 1;
}
