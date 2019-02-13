/*
    MIT License

    Copyright (c) 2019, Alexey Dynda

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
 * @file menu.h Menu class
 */

#ifndef _NANO_MENU_H_
#define _NANO_MENU_H_

#include "point.h"
#include "object.h"
#include "rect.h"
#include "ssd1306_hal/io.h"

/**
 * @ingroup NANO_ENGINE_API
 * @{
 */

/**
 * This is base class for user menu implementations.
 * NanoMenu can work only as part of NanoEngine.
 */
class NanoMenu: public NanoObject
{
public:
    /**
     * Creates menu object.
     * @param pos position of the sprite in global coordinates
     * @param bitmap sprite content (in flash memory)
     */
    NanoMenu(const NanoPoint &pos )
         : NanoObject( pos )
    {
    }

    /**
     * Draws monochrome sprite on Engine canvas
     */
    void draw() override {}

    void refresh() override {}

    void update() override {}
};

/**
 * @}
 */

#endif

