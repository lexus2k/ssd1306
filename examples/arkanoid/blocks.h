/*
    Copyright (C) 2016-2017 Alexey Dynda

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

enum
{
    NO_BLOCK,     // 0
    BLOCK_SIMPLE, // 1
    BLOCK_DASHED, // 2
    BLOCK_STRONG, // 3

    BLOCK_BOMB,   // 4
    BLOCK_HEART,  // 5
    BLOCK_POWER,  // 6
    BLOCKS_MAX,
    BLOCK_BONUS = BLOCK_BOMB,
};


const PROGMEM uint8_t blockImages[BLOCKS_MAX][16] =
{
    {   /* EMPTY */
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000
    },
    {   /* SIMPLE */
        B00000000,
        B00111100,
        B01111110,
        B01100110,
        B01100110,
        B01100110,
        B01100110,
        B01100110,
        B01100110,
        B01100110,
        B01100110,
        B01100110,
        B01100110,
        B01111110,
        B00111100,
        B00000000
    },
    {   /* DASHED */
        B00000000,
        B00111100,
        B01010010,
        B01001010,
        B01100110,
        B01010010,
        B01001010,
        B01100110,
        B01010010,
        B01001010,
        B01100110,
        B01010010,
        B01001010,
        B01100110,
        B00111100,
        B00000000
    },
    {   /* STRONG */
        B00000000,
        B00111100,
        B01111110,
        B01111110,
        B01111110,
        B01111110,
        B01111110,
        B01111110,
        B01111110,
        B01111110,
        B01111110,
        B01111110,
        B01111110,
        B01111110,
        B00111100,
        B00000000
    },
    {   /* BOMB */
        B00000000,
        B00111100,
        B01111110,
        B01111110,
        B01011010,
        B01011010,
        B01100110,
        B01100110,
        B01011010,
        B01011010,
        B01111110,
        B01111110,
        B01111010,
        B01110110,
        B00111100,
        B00000000
    },
    {   /* HEART */
        B00000000,
        B00111100,
        B01111110,
        B01111110,
        B01111110,
        B01110110,
        B01100010,
        B01000110,
        B01100010,
        B01110110,
        B01111110,
        B01111110,
        B01111110,
        B01111110,
        B00111100,
        B00000000
    },
    {   /* POWER */
        B00000000,
        B00111100,
        B01111110,
        B01111110,
        B01000010,
        B01101010,
        B01101010,
        B01101010,
        B01101010,
        B01110110,
        B01111110,
        B01111110,
        B01111010,
        B01110110,
        B00111100,
        B00000000
    },
};

