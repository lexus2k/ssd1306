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

#include "ninja.h"
#include "sprites.h"

#include "intf/ssd1306_interface.h"
#include "intf/spi/ssd1306_spi.h"

Ninja ninja({72, 8});

void Ninja::move(const NanoPoint &target)
{
    static uint16_t ninjaAnimationTs = 0;
    static uint8_t  ninjaAnimation = 0;
    bool animated = false;
    uint8_t direction = BUTTON_NONE;
    uint8_t bottomBlock = block_at(bottom());
    uint8_t feetBlock = block_at(bottom() + (NanoPoint){0,1});
    uint8_t handBlock = block_at(top());
    uint8_t centerBlock = block_at(center());
    uint8_t rightBlock = block_at(right());
    uint8_t leftBlock = block_at(left());
    if ( !isSolid(feetBlock) &&
         (!isPipe(handBlock) || !isPipe(bottomBlock)) )
    {
        moveTo( { center().x & ~0x07, m_pos.y + 1 } );
        setBitmap( &playerFlyingImage[MAN_ANIM_FLYING][ninjaAnimation][0] );
        animated = true;
    }
    else
    {
        if (target.y < m_pos.y - 1)
        {
            bool right = true;
            bool left = true;
            // search for stairs
            for (int8_t i=0; i < 80; i=i+8)
            {
                if (right)
                {
                    uint8_t block = block_at(center() + (NanoPoint){i,0});
                    if (!isWalkable(block)) { right = false; }
                    if (isStair(block)) { direction = BUTTON_RIGHT; break; }
                }
                if (left)
                {
                    uint8_t block = block_at(center() - (NanoPoint){i,0});
                    if (!isWalkable(block)) { left = false; }
                    if (isStair(block)) { direction = BUTTON_LEFT; break; }
                }
            }
            if (isStair(centerBlock) || isStair(bottomBlock)) direction = BUTTON_UP;
        }
        else if (target.y > m_pos.y + 1)
        {
            if (isPipe(handBlock))
            {
                direction = BUTTON_DOWN;
            }
            else
            {
                bool right = true;
                bool left = true;
                // search for stairs
                for (int8_t i=0; i < 80; i=i+8)
                {
                    if (right)
                    {
                        uint8_t block = block_at(center() + (NanoPoint){i,0});
                        if (!isWalkable(block)) right = false;
                        else
                        {
                            block = block_at(bottom() + (NanoPoint){i,1});
                            if (isWalkable(block)) { direction = BUTTON_RIGHT; break; }
                        }
                    }
                    if (left)
                    {
                        uint8_t block = block_at(center() - (NanoPoint){i,0});
                        if (!isWalkable(block)) left = false;
                        else
                        {
                            block = block_at(bottom() + (NanoPoint){-i,1});
                            if (isWalkable(block)) { direction = BUTTON_LEFT; break; }
                        }
                    }
                }
                if (isWalkable(feetBlock)) direction = BUTTON_DOWN;
            }
        }
        else if (target.x > m_pos.x)
        {
            if (isWalkable(rightBlock)) direction = BUTTON_RIGHT;
        }
        else if (target.x < m_pos.x)
        {
            if (isWalkable(leftBlock)) direction = BUTTON_LEFT;
        }
        switch (direction)
        {
            case BUTTON_RIGHT:
                moveBy( { 1, 0 } );
                if (isPipe(centerBlock))
                    setBitmap( &playerFlyingImage[MAN_ANIM_RIGHT_PIPE][ninjaAnimation][0] );
                else
                    setBitmap( &playerFlyingImage[MAN_ANIM_RIGHT][ninjaAnimation][0] );
                animated = true;
                break;
            case BUTTON_LEFT:
                moveBy( { -1, 0 } );
                if (isPipe(centerBlock))
                    setBitmap( &playerFlyingImage[MAN_ANIM_LEFT_PIPE][ninjaAnimation][0] );
                else
                    setBitmap( &playerFlyingImage[MAN_ANIM_LEFT][ninjaAnimation][0] );
                animated = true;
                break;
            case BUTTON_UP:
                moveTo( { top().x & ~0x07, top().y - 1 } );
                setBitmap( &playerFlyingImage[MAN_ANIM_UP][ninjaAnimation][0] );
                animated = true;
                break;
            case BUTTON_DOWN:
                moveTo( { top().x & ~0x07, top().y + 1 } );
                setBitmap( &playerFlyingImage[MAN_ANIM_DOWN][ninjaAnimation][0] );
                animated = true;
                break;
            default:
                break;
        }
    }
    if (animated && ((uint16_t)(millis() - ninjaAnimationTs) > 150))
    {
        ninjaAnimationTs = millis();
        ninjaAnimation = ninjaAnimation ? 0 : 1;
    }
}

