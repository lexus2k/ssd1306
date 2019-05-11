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
 * @file point.h Point class
 */


#ifndef _NANO_POINT_H_
#define _NANO_POINT_H_

#include "ssd1306_hal/io.h"

/**
 * @ingroup NANO_ENGINE_API
 * @{
 */

/** Describes point */
typedef struct _NanoPoint
{
    /** x position in pixels */
    lcdint_t x;
    /** y position in pixels */
    lcdint_t y;

//    _NanoPoint(lcdint_t px, lcdint_t py)
//    {
//        x = px;
//        y = py;
//    }

    /**
     * Initializes NanoPoint with specified values
     * @param px - x position
     * @param py - y position
     */
    void setPoint(lcdint_t px, lcdint_t py) { x=px; y=py; };

    /**
     * Shifts right x,y value of the point by bits value.
     * @param bits - number of bits to shift
     */
    _NanoPoint& operator>>=(const uint8_t bits)
    {
        x >>= bits;
        y >>= bits;
        return *this;
    }

    /**
     * Shifts left x,y value of the point by bits value.
     * @param bits - number of bits to shift
     */
    _NanoPoint& operator<<=(const uint8_t bits)
    {
        x <<= bits;
        y <<= bits;
        return *this;
    }

    /**
     * Adds point.
     * @param p - point values to add to the point.
     */
    _NanoPoint& operator+=(const _NanoPoint &p)
    {
        x += p.x;
        y += p.y;
        return *this;
    };

    /**
     * Subtracts point.
     * @param p - point values to subtract from the point.
     */
    _NanoPoint& operator-=(const _NanoPoint &p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    };

    /**
     * Subtracts point.
     * @param p - point values to subtract from the point.
     */
    _NanoPoint operator-(const _NanoPoint &p)
    {
        return { static_cast<lcdint_t>(x - p.x),
                 static_cast<lcdint_t>(y - p.y) };
    };

    /**
     * Adds point.
     * @param p - point values to add to the point.
     */
    _NanoPoint operator+(const _NanoPoint &p)
    {
        return { static_cast<lcdint_t>(x + p.x),
                 static_cast<lcdint_t>(y + p.y) };
    };

    /**
     * Shifts right x,y value of the point by bits value.
     * @param bits - number of bits to shift
     */
    _NanoPoint operator>>(const uint8_t bits) const
    {
        return { static_cast<lcdint_t>(x >> bits),
                 static_cast<lcdint_t>(y >> bits) };
    };

    /**
     * Shifts left x,y value of the point by bits value.
     * @param bits - number of bits to shift
     */
    _NanoPoint operator<<(const uint8_t bits) const
    {
        return { static_cast<lcdint_t>(x << bits),
                 static_cast<lcdint_t>(y << bits) };
    };

    /**
     * Divides x,y value of the point by d value.
     * @param d divider
     */
    _NanoPoint operator/(const int16_t d) const
    {
        return { static_cast<lcdint_t>(x / d),
                 static_cast<lcdint_t>(y / d) };
    };

} NanoPoint;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
inline NanoPoint operator+(const NanoPoint& p1, const NanoPoint& p2)
{
    return { (lcdint_t)(p1.x + p2.x), (lcdint_t)(p1.y + p2.y) };
}

inline NanoPoint operator-(const NanoPoint& p1, const NanoPoint& p2)
{
    return { (lcdint_t)(p1.x - p2.x), (lcdint_t)(p1.y - p2.y) };
}

inline bool operator==(const NanoPoint& p1, const NanoPoint& p2)
{
    return (p1.x == p2.x) && (p1.y == p2.y);
}

inline bool operator!=(const NanoPoint& p1, const NanoPoint& p2)
{
    return !(p1 == p2);
}
#endif // DOXYGEN_SHOULD_SKIP_THIS

/**
 * @}
 */

#endif

