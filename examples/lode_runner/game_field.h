/*
    Copyright (C) 2017 Alexey Dynda

    This file is part of SSD1306 library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
