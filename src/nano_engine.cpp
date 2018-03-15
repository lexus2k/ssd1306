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
#include "ssd1306.h"

extern SFixedFontInfo s_fixedFont;

///////////////////////////////////////////////////////////////////////////////
////// NANO ENGINE RGB        /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

NanoCanvas8 NanoEngine8::canvas;

//uint8_t NanoEngine8::m_buffer[NE_TILE_SIZE * NE_TILE_SIZE];

NanoEngine8::NanoEngine8()
   : NanoEngineBase()
   , NanoEngineTiler()
{
}

void NanoEngine8::begin()
{
    // TODO: Maybe, this is not good place for this, but
    // TODO: ssd1331 must be initialized in Horizontal addressing mode
    ssd1331_setMode(0);
    canvas.begin(NE_TILE_WIDTH, NE_TILE_HEIGHT, m_buffer);
    NanoEngineBase::begin();
}

void NanoEngine8::display()
{
    m_lastFrameTs = millis();
    for (uint8_t y = 0; y < s_displayHeight; y = y + NE_TILE_HEIGHT)
    {
        uint16_t flag = m_refreshFlags[y >> NE_TILE_SIZE_BITS];
        m_refreshFlags[y >> NE_TILE_SIZE_BITS] = 0;
        for (uint8_t x = 0; x < s_displayWidth; x = x + NE_TILE_WIDTH)
        {
            if (flag & 0x01)
            {
                canvas.setOffset(x, y);
                if ((m_onDraw) && (m_onDraw()))
                {
                    canvas.blt();
                }
            }
            flag >>=1;
        }
    }
    m_cpuLoad = ((millis() - m_lastFrameTs)*100)/m_frameDurationMs;
}

void NanoEngine8::notify(const char *str)
{
    NanoRect rect = { 8, (s_displayHeight>>1) - 8, s_displayWidth - 8, (s_displayHeight>>1) + 8 };
    // TODO: It would be nice to calculate message height
    NanoPoint textPos = { (s_displayWidth - (lcdint_t)strlen(str)*s_fixedFont.width) >> 1, (s_displayHeight>>1) - 4 };
    refresh(rect);
    for (uint8_t y = 0; y < s_displayHeight; y = y + NE_TILE_HEIGHT)
    {
        uint16_t flag = m_refreshFlags[y >> NE_TILE_SIZE_BITS];
        m_refreshFlags[y >> NE_TILE_SIZE_BITS] = 0;
        for (uint8_t x = 0; x < s_displayWidth; x = x + NE_TILE_WIDTH)
        {
            if (flag & 0x01)
            {
                canvas.setOffset(x, y);
                if (m_onDraw) m_onDraw();
                canvas.setColor(RGB_COLOR8(0,0,0));
                canvas.fillRect(rect);
                canvas.setColor(RGB_COLOR8(192,192,192));
                canvas.drawRect(rect);
                canvas.printFixed( textPos.x, textPos.y, str);

                canvas.blt();
            }
            flag >>=1;
        }
    }
    delay(1000);
    m_lastFrameTs = millis();
    refresh();
}

///////////////////////////////////////////////////////////////////////////////
////// NANO ENGINE MONOCHROME /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** object, representing canvas. Use it in your draw handler */
NanoCanvas1 NanoEngine1::canvas;

//uint8_t   NanoEngine1::m_buffer[NE_TILE_SIZE * NE_TILE_SIZE / 8];

NanoEngine1::NanoEngine1()
   : NanoEngineBase()
   , NanoEngineTiler()
{
}

void NanoEngine1::begin()
{
    canvas.begin(NE_TILE_WIDTH, NE_TILE_HEIGHT, m_buffer);
    NanoEngineBase::begin();
}

void NanoEngine1::display()
{
    m_lastFrameTs = millis();
    for (uint8_t y = 0; y < s_displayHeight; y = y + NE_TILE_HEIGHT)
    {
        uint16_t flag = m_refreshFlags[y >> NE_TILE_SIZE_BITS];
        m_refreshFlags[y >> NE_TILE_SIZE_BITS] = 0;
        for (uint8_t x = 0; x < s_displayWidth; x = x + NE_TILE_WIDTH)
        {
            if (flag & 0x01)
            {
                canvas.setOffset(x, y);
                if (m_onDraw)
                {
                    if (m_onDraw()) canvas.blt();
                }
            }
            flag >>=1;
        }
    }
    m_cpuLoad = ((millis() - m_lastFrameTs)*100)/m_frameDurationMs;
}

void NanoEngine1::notify(const char *str)
{
    NanoRect rect = { 8, (s_displayHeight>>1) - 8, s_displayWidth - 8, (s_displayHeight>>1) + 8 };
    // TODO: It would be nice to calculate message height
    NanoPoint textPos = { (s_displayWidth - (lcdint_t)strlen(str)*s_fixedFont.width) >> 1, (s_displayHeight>>1) - 4 };
    refresh(rect);
    for (uint8_t y = 0; y < s_displayHeight; y = y + NE_TILE_HEIGHT)
    {
        uint16_t flag = m_refreshFlags[y >> NE_TILE_SIZE_BITS];
        m_refreshFlags[y >> NE_TILE_SIZE_BITS] = 0;
        for (uint8_t x = 0; x < s_displayWidth; x = x + NE_TILE_WIDTH)
        {
            if (flag & 0x01)
            {
                canvas.setOffset(x, y);
                if (m_onDraw) m_onDraw();
                canvas.setColor(RGB_COLOR8(0,0,0));
                canvas.fillRect(rect);
                canvas.setColor(RGB_COLOR8(192,192,192));
                canvas.drawRect(rect);
                canvas.printFixed( textPos.x, textPos.y, str);

                canvas.blt();
            }
            flag >>=1;
        }
    }
    delay(1000);
    m_lastFrameTs = millis();
    refresh();
}

