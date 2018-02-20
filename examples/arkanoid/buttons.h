/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

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

#pragma once

#include <stdint.h>

#if defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__)
#    define LEFT_BTN    2
#    define RIGHT_BTN   0
#else // For Arduino Nano/Atmega328 we use different pins
#    define USE_Z_KEYPAD // use analog Z-keypad ADC module on A0 pin.
#    ifndef USE_Z_KEYPAD
#        define LEFT_BTN    4
#        define RIGHT_BTN   2
#    endif
#endif

const uint8_t BUTTON_NONE   = 0;
const uint8_t BUTTON_RIGHT  = 1;
const uint8_t BUTTON_UP     = 2;
const uint8_t BUTTON_DOWN   = 3;
const uint8_t BUTTON_LEFT   = 4;
const uint8_t BUTTON_SELECT = 5;

uint8_t getPressedButton(uint8_t analogPin);

