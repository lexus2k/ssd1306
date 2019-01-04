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

#pragma once

#include "ssd1306.h"
#include "nano_engine.h"

#define B_WIDTH  30

typedef NanoEngine<TILE_16x16_RGB8> GraphicsEngine;

extern uint8_t gameField[];
extern GraphicsEngine engine;

static inline bool isWalkable(uint8_t type)          { return (type == 0) || (type == 2) || (type == 3) || (type == 4); }
static inline bool isSolid(uint8_t type)             { return (type == 1) || (type == 2) || (type == 5); }
static inline bool isPipe(uint8_t type)              { return type == 3; }
static inline bool isGold(uint8_t type)              { return type == 4; }
static inline bool isStair(uint8_t type)             { return type == 2; }

static inline uint16_t block_index(const NanoPoint& block)
{
    return block.x + block.y * B_WIDTH;
}

static inline NanoPoint pos_to_block(const NanoPoint& pos)
{
    return pos >> 3;
}

static inline NanoPoint block_to_pos(const NanoPoint& block)
{
    return block << 3;
}

static inline NanoRect rect_to_blocks(const NanoRect& rect)
{
    return rect >> 3;
}

static inline uint8_t block_value(const NanoPoint& block)
{
    uint16_t index = block_index(block);
    if (index >= B_WIDTH*14) index = 0;
    return gameField[index];
}

static inline uint8_t block_at(const NanoPoint& p)
{
    return block_value(pos_to_block(p));
}

static inline void set_block_at(const NanoPoint& p, uint8_t v)
{
    uint16_t index = block_index(pos_to_block(p));
    if (index >= B_WIDTH*14) index = 0;
    gameField[index] = v;
}

