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

///////////////////////////////////////////////////////////////////////////////
////// NANO ENGINE RGB        /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

NanoEngine8::NanoEngine8()
   : NanoEngineBase()
   , canvas(1<<NE_TILE_SIZE_BITS, 1<<NE_TILE_SIZE_BITS, m_buffer)
   , m_buffer{}
{
}

void NanoEngine8::display()
{
    if (m_loop) m_loop();
    m_lastFrameTs = millis();
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
    m_cpuLoad = ((millis() - m_lastFrameTs)*100)/m_frameDurationMs;
}

///////////////////////////////////////////////////////////////////////////////
////// NANO ENGINE BASE CLASS /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

NanoEngineBase::NanoEngineBase()
   : m_frameDurationMs(33)
   , m_fps(30)
   , m_cpuLoad(0)
   , m_lastFrameTs(0)
   , m_onDraw( nullptr )
   , m_onButtons( nullptr )
   , m_loop( nullptr )
{
    refreshAll();
}

void NanoEngineBase::begin()
{
    m_lastFrameTs = millis();
}

void NanoEngineBase::setFrameRate(uint8_t fps)
{
    m_fps = fps;
    m_frameDurationMs = 1000/fps;
}

bool NanoEngineBase::nextFrame()
{
    return (uint32_t)(millis() - m_lastFrameTs) >= m_frameDurationMs;
}

void NanoEngineBase::refreshRect(NanoRect &rect)
{
    refreshRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

void NanoEngineBase::refreshRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    for(uint8_t y=(y1>>NE_TILE_SIZE_BITS); y<=(y2>>NE_TILE_SIZE_BITS); y++)
    {
        for(uint8_t x=(x1>>NE_TILE_SIZE_BITS); x<=(x2>>NE_TILE_SIZE_BITS); x++)
        {
            m_refreshFlags[y] |= (1<<x);
        }
    }
}

void NanoEngineBase::refreshPoint(NanoPoint &point)
{
    m_refreshFlags[(point.y>>NE_TILE_SIZE_BITS)] |= (1<<(point.x>>NE_TILE_SIZE_BITS));
}

void NanoEngineBase::refreshAll()
{
    for(uint8_t i=0; i<NE_MAX_TILES_NUM; i++)
    {
        m_refreshFlags[i] = ~(0);
    }
}

bool NanoEngineBase::pressed(uint8_t buttons)
{
    return (m_onButtons() & buttons) == buttons;
}

bool NanoEngineBase::notPressed(uint8_t buttons)
{
    return (m_onButtons() & buttons) == 0;
}

void NanoEngineBase::connectCustomKeys(TNanoEngineGetButtons handler)
{
    m_onButtons = handler;
}

void NanoEngineBase::connectArduboyKeys()
{
    m_onButtons = arduboyButtons;
}

uint8_t NanoEngineBase::s_zkeypadPin;

uint8_t NanoEngineBase::zkeypadButtons()
{
    int buttonValue = analogRead(s_zkeypadPin);
    if (buttonValue < 100) return BUTTON_RIGHT;  
    if (buttonValue < 200) return BUTTON_UP;
    if (buttonValue < 400) return BUTTON_DOWN;
    if (buttonValue < 600) return BUTTON_LEFT;
    if (buttonValue < 800) return BUTTON_A;
    /** Z-keypad has only 5 analog buttons: no button B */
    return BUTTON_NONE;
}

void NanoEngineBase::connectZKeypad(uint8_t analogPin)
{
    NanoEngineBase::s_zkeypadPin = analogPin;
    m_onButtons = zkeypadButtons;
}

uint8_t NanoEngineBase::arduboyButtons()
{
    uint8_t buttons;
    /* Arduboy buttons available only for Atmega32U4 platform */
    #if defined(__AVR_ATmega32U4__)
    // down, up, left right
    buttons = (((~PINF) & 0B11110000)>>4);
    // A (left)
    buttons |= (((~PINE) & 0B01000000) >> 2);
    // B (right)
    buttons |= (((~PINB) & 0B00010000) << 1);
    #else
    buttons = 0;
    #endif
    return buttons;
}
