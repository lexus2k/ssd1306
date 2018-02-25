/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

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

#include "nano_engine.h"
#include "lcd/lcd_common.h"


NanoEngine8::NanoEngine8()
   : canvas(NE_TILE_SIZE, NE_TILE_SIZE, m_buffer)
   , m_buffer{}
   , m_onDraw( nullptr )
{
    refreshAll();
}

void NanoEngine8::display()
{
    for (uint8_t y = 0; y < (s_displayHeight >> NE_TILE_SIZE_BITS); y++)
    {
        for (uint8_t x = 0; x < (s_displayWidth >> NE_TILE_SIZE_BITS); x++)
        {
            if (m_refreshFlags[y] & (1<<x))
            {
                canvas.setOffset(x<<NE_TILE_SIZE_BITS, y<<NE_TILE_SIZE_BITS);
                if (m_onDraw)
                {
                    if (m_onDraw()) canvas.blt();
                }
            }
        }
        m_refreshFlags[y] = 0;
    }
}

void NanoEngine8::refreshAll()
{
    for(uint8_t i=0; i<NE_MAX_TILES_NUM; i++)
    {
        m_refreshFlags[i] = 0xFF;
    }
}

