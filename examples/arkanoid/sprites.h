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

const PROGMEM uint8_t heartSprite[8] =
{
    B00001110,
    B00011111,
    B00111111,
    B01111110,
    B01111110,
    B00111101,
    B00011001,
    B00001110
};


const PROGMEM uint8_t powerSprite[8] =
{
    B11001110,
    B11111110,
    B00111110,
    B00010110,
    B00001110,
    B00001110,
    B00001111,
    B00001110
};


const PROGMEM uint8_t shootSprite[1] =
{
    B00111100,
};

const PROGMEM uint8_t bonusSprites[][5] =
{
    { /* BLOCK_BOMB */
        B01111001,
        B11111110,
        B11101110,
        B01111001,
        B00000000,
    },
    { /* BLOCK_HEART */
        B00011100,
        B00111110,
        B01111100,
        B00111010,
        B00011100,
    },
    { /* BLOCK_POWER */
        B00000000,
        B01111110,
        B00010010,
        B00010010,
        B00001100,
    },
};


