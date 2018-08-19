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
 * @file sprite.h Sprite class
 */

#ifndef _NANO_OBJECT_H_
#define _NANO_OBJECT_H_

#include "point.h"
#include "rect.h"
#include "ssd1306_hal/io.h"

/**
 * @ingroup NANO_ENGINE_API
 * @{
 */

/**
 * This is base class for all NanoObjects.
 */
class NanoObject
{
public:
    /**
     * Creates basic object.
     */
    NanoObject(): default;

    /**
     * Draws nano object Engine canvas
     */
    virtual void draw() = 0;

    /**
     * Marks nano object location for refreshing on the new frame
     */
    virtual void refresh() = 0;
};

class NanoSquareObject
{
};

/**
 * @}
 */

#endif

