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

#include "core.h"

///////////////////////////////////////////////////////////////////////////////
////// NANO ENGINE INPUTS CLASS ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** Callback to call if buttons state needs to be updated */
TNanoEngineGetButtons NanoEngineInputs::m_onButtons = nullptr;

bool NanoEngineInputs::pressed(uint8_t buttons)
{
    return (m_onButtons() & buttons) == buttons;
}

bool NanoEngineInputs::notPressed(uint8_t buttons)
{
    return (m_onButtons() & buttons) == 0;
}

void NanoEngineInputs::connectCustomKeys(TNanoEngineGetButtons handler)
{
    m_onButtons = handler;
}

void NanoEngineInputs::connectArduboyKeys()
{
    m_onButtons = arduboyButtons;
}

uint8_t NanoEngineInputs::s_zkeypadPin;

uint8_t NanoEngineInputs::zkeypadButtons()
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

void NanoEngineInputs::connectZKeypad(uint8_t analogPin)
{
    NanoEngineInputs::s_zkeypadPin = analogPin;
    m_onButtons = zkeypadButtons;
}

uint8_t NanoEngineInputs::arduboyButtons()
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
