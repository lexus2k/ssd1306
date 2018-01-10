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

#include "sprite_pool.h"
#include "level_sprites.h"
#include <stdint.h>

extern uint8_t gameField[];

class GameField: public SpritePool
{
public:
    virtual void drawBlock(uint8_t column, uint8_t row)
    {
        uint8_t index = ((row - 1) << 4) + column;
        if (gameField[index] == 0)
        {
            m_canvas.clear();
        }
        else
        {
            m_canvas.clear();
            m_canvas.drawSpritePgm(0,0,bgSprites[gameField[index] - 1]);
        }
    }
};

extern GameField field;

static inline uint8_t blockIdx(uint8_t x, uint8_t y)
{
    return (x>>3) + (((y - 8)>>3) << 4);
}

static inline bool isWalkable(uint8_t type)
{
    return (type == 0) || (type == 2) || (type == 3) || (type == 4);
}

static inline bool isSolid(uint8_t type)
{
    return (type == 1) || (type == 2) || (type == 5);
}

static inline bool isPipe(uint8_t type)
{
    return type == 3;
}

static inline bool isGold(uint8_t type)
{
    return type == 4;
}

static inline bool isStair(uint8_t type)
{
    return type == 2;
}
