/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

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

#ifndef _NANO_SPRITE_H_
#define _NANO_SPRITE_H_

#include "canvas/point.h"
#include "canvas/rect.h"
#include "object.h"
#include "ssd1306_hal/io.h"

/**
 * @ingroup NANO_ENGINE_API
 * @{
 */

/**
 * This is template class for user sprites implementations.
 * NanoSprite can work only as part of NanoEngine.
 */
template<class T>
class NanoSprite: public NanoObject<T>
{
public:
    /**
     * Creates sprite object of variable size. Such sprites can
     * change their size and bitmap content.
     * @param pos position of the sprite in global coordinates
     * @param size size of sprite
     * @param bitmap sprite content (in flash memory)
     */
    NanoSprite(const NanoPoint &pos, const NanoPoint &size, const uint8_t *bitmap)
         : NanoObject<T>( pos, size )
         , m_bitmap( bitmap )
    {
    }

    void draw() override
    {
        this->getTiler().getCanvas().drawBitmap1(
            this->getRect().p1.x, this->getRect().p1.y,
            this->getRect().width(), this->getRect().height(),
            this->getBitmap());
    }

    /**
     * Replaces sprite bitmap with new one.
     */
    void setBitmap(const uint8_t *bitmap) { m_bitmap = bitmap; }

    /**
     * Returns pointer to sprite bitmap data.
     * The data are located in the Flash memory.
     */
    const uint8_t *getBitmap() { return m_bitmap; }

private:
    const uint8_t *m_bitmap;
};

/**
 * This is base class for user sprites implementation.
 */
template<class T>
class NanoFixedSprite: public NanoSprite<T>
{
public:
    using NanoSprite<T>::NanoSprite;
};

/**
 * @}
 */

#endif

