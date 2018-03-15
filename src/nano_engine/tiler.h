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
 * @file tiler.h Tiler helper for graphics processing
 */


#ifndef _NANO_ENGINE_TILER_H_
#define _NANO_ENGINE_TILER_H_

#include "nano_canvas.h"

/* The table below defines arguments for NanoEngineTiler.          *
 *                            canvas        width   height  bits   */
#define TILE_8x8_RGB8         NanoCanvas8,  8,      8,      3
#define TILE_8x8_MONO         NanoCanvas1,  8,      8,      3

/**
 * This class template is responsible for holding and updating data about areas to be refreshed
 * on LCD display. It accepts canvas class, tile width in pixels, tile height in pixels and
 * number of bits in tile width as arguments for the template.
 * For example, for 8x8 8-bit RGB tiles the reference should be NanoEngineTiler<NanoCanvas8,8,8,3>,
 * and 3 bits means 3^2 = 8.
 * If you need to have single big buffer, holding the whole content for monochrome display,
 * you can specify something like this NanoEngineTiler<NanoCanvas1,128,64,7>.
 */
template<class C, uint8_t W, uint8_t H, uint8_t B>
class NanoEngineTiler
{
protected:
    /* Only child classes can initialize the engine */
    NanoEngineTiler() { refresh(); };

public:
    /** Number of bits in tile size. 5 corresponds to 1<<5 = 32 tile size */
    static const uint8_t NE_TILE_SIZE_BITS = B;
    /** Width of tile in pixels */
    static const uint8_t NE_TILE_WIDTH = W;
    /** Height of tile in pixels */
    static const uint8_t NE_TILE_HEIGHT = H;
    /** Max tiles supported in X */
    static const uint8_t NE_MAX_TILES_NUM = 16 >> (B - 3);

    /**
     * Marks all tiles for update. Actual update will take place in display() method.
     */
    static void refresh()
    {
        for(uint8_t i=0; i<NanoEngineTiler<C,W,H,B>::NE_MAX_TILES_NUM; i++)
        {
            m_refreshFlags[i] = ~(0);
        }
    }

    /**
     * Mark specified area in pixels for redrawing by NanoEngine.
     * Actual update will take place in display() method.
     */
    static void refresh(const NanoRect &rect)
    {
        refresh(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
    }

    /**
     * Mark specified area in pixels for redrawing by NanoEngine.
     * Actual update will take place in display() method.
     */
    static void refresh(const NanoPoint &point)
    {
        if ((point.y<0) || ((point.y>>B)>=NE_MAX_TILES_NUM)) return;
        m_refreshFlags[(point.y>>B)] |= (1<<(point.x>>B));
    }

    /**
     * Mark specified area in pixels for redrawing by NanoEngine.
     * Actual update will take place in display() method.
     */
    static void refresh(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
    {
        y1 = max(0,y1>>B);
        y2 = min((y2>>B), NE_MAX_TILES_NUM - 1);
        for(uint8_t y=y1; y<=y2; y++)
        {
            for(uint8_t x=(x1>>B); x<=(x2>>B); x++)
            {
                m_refreshFlags[y] |= (1<<x);
            }
        }
    }

    /**
     * @brief Returns true if point is inside the rectangle area.
     * Returns true if point is inside the rectangle area.
     * @param p - point to check
     * @param rect - rectangle, describing the region to check with the point
     * @returns true if point is inside the rectangle area.
     */
    static bool collision(NanoPoint &p, NanoRect &rect) { return rect.collision( p ); }

protected:
    /**
     * Contains information on tiles to be updated.
     * Elements of array are rows and bits are columns.
     */
    static uint16_t   m_refreshFlags[NE_MAX_TILES_NUM];

    static uint8_t    m_buffer[W * H * C::BITS_PER_PIXEL / 8];
};

template<class C, uint8_t W, uint8_t H, uint8_t B>
uint16_t NanoEngineTiler<C,W,H,B>::m_refreshFlags[NE_MAX_TILES_NUM];

template<class C, uint8_t W, uint8_t H, uint8_t B>
uint8_t NanoEngineTiler<C,W,H,B>::m_buffer[W * H * C::BITS_PER_PIXEL / 8];

#endif

