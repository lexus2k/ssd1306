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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/* This is special case for non-Arduino platforms, since Adafruit requires *
 * Arduino libraries support                                               */
#ifndef ARDUINO
#define ARDUINO 100
#include "Adafruit_GFX.h"
#undef ARDUINO
#else
#include "Adafruit_GFX.h"
#endif

#endif // DOXYGEN_SHOULD_SKIP_THIS

/**
 * This is basic template class for all canvas classes,
 * based on Adafruit_GFX. This base class provides functionality
 * compatible with native NanoCanvas implementation of ssd1306
 * library
 */
template <uint8_t BPP>
class AdafruitCanvasOps: public Adafruit_GFX
{
public:
    /** Fixed offset for all operation of NanoCanvasOps in pixels. */
    NanoPoint offset;

    /** number of bits per single pixel in buffer */
    static const uint8_t BITS_PER_PIXEL = BPP;

    /**
     * Initializes canvas, based on Adafruit GFX.
     * @param w width of canvas
     * @param h height of canvas area
     * @param buffer buffer to use for pixels
     *
     * @note the size of buffer must be enough to store (w*h*bpp/8) bytes.
     */
    AdafruitCanvasOps(lcduint_t w, lcduint_t h, uint8_t *buffer)
        : Adafruit_GFX(w, h)
        , offset{0}
        , m_buffer(buffer)
    {
    }

    /**
     * draw single pixel in canvas area
     *
     * @param x x position
     * @param y y position
     * @param color color of pixel: for monochrome it can be 0 (black),
     *        1 (white), 2 (invert)
     */
    void drawPixel(int16_t x, int16_t y, uint16_t color) override;

protected:

    /** pixels buffer */
    uint8_t *m_buffer;
};

template <>
void AdafruitCanvasOps<1>::drawPixel(int16_t x, int16_t y, uint16_t color)
{
        x -= offset.x;
        y -= offset.y;
        if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
        {
            return;
        }

        switch (getRotation()) {
        case 1:
            ssd1306_swap_data(x, y, int16_t);
            x = width() - x - 1;
            break;
        case 2:
            x = width() - x - 1;
            y = height() - y - 1;
            break;
        case 3:
            ssd1306_swap_data(x, y, int16_t);
            y = height() - y - 1;
            break;
        }

        switch (color)
        {
            case 1:   m_buffer[x+ (y/8)*width()] |=  (1 << (y&7)); break;
            case 0:   m_buffer[x+ (y/8)*width()] &= ~(1 << (y&7)); break;
            case 2:   m_buffer[x+ (y/8)*width()] ^=  (1 << (y&7)); break;
        }
}


/**
 * AdafruitCanvas1 represents objects for drawing in memory buffer
 * AdafruitCanvas1 represents each pixel as single bit: 0/1
 * For details refer to SSD1306 datasheet
 */
class AdafruitCanvas1 : public AdafruitCanvasOps<1>
{
public:
    using AdafruitCanvasOps::AdafruitCanvasOps;
};


#endif // CONFIG_ADAFRUIT_GFX_ENABLE

#endif
