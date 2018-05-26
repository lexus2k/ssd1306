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
/**
 * @file adafruit.h Adafruit related canvas implementation
 */
#ifndef _SSD1306_ADAFRUIT_H_
#define _SSD1306_ADAFRUIT_H_

#include "ssd1306_hal/io.h"

#if defined(CONFIG_ADAFRUIT_GFX_ENABLE)

#include "ssd1306_hal/Print_internal.h"
#include "nano_gfx_types.h"

/* This is special case for non-Arduino platforms, since Adafruit requires *
 * Arduino libraries support                                               */
#ifndef ARDUINO
#define ARDUINO 100
#include "Adafruit_GFX.h"
#undef ARDUINO
#else
#include "Adafruit_GFX.h"
#endif

class AdafruitCanvas1 : public Adafruit_GFX
{
public:
    AdafruitCanvas1(lcduint_t w, lcduint_t h, uint8_t *buffer): Adafruit_GFX(w, h), m_buffer(buffer) {}
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color)
    {
        if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
        {
            return;
        }

        switch (color)
        {
            case 1:   m_buffer[x+ (y/8)*width()] |=  (1 << (y&7)); break;
            case 0:   m_buffer[x+ (y/8)*width()] &= ~(1 << (y&7)); break;
            case 2:   m_buffer[x+ (y/8)*width()] ^=  (1 << (y&7)); break;
        }
    }
private:
    uint8_t *m_buffer;
};


#endif // CONFIG_ADAFRUIT_GFX_ENABLE

#endif
