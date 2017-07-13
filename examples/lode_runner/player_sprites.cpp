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

#include "player_sprites.h"

const PROGMEM uint8_t playerFlyingImage[MAN_ANIM_MAX][2][8] = {
{
    {
        B00000000,
        B00001000,
        B10010000,
        B01110100,
        B00111110,
        B01110100,
        B10010000,
        B00001000,
    },
    {
        B00000000,
        B00100000,
        B10010000,
        B01110100,
        B00111110,
        B01110100,
        B10010000,
        B00100000,
    },
},
{
    {
        B00100000,
        B10010000,
        B01110100,
        B00111110,
        B01110100,
        B01010000,
        B01001000,
        B00000000,
    },
    {
        B01001000,
        B01010000,
        B01110100,
        B00111110,
        B01110100,
        B10010000,
        B00100000,
        B00000000,
    },
},
{
    {
        B00000000,
        B00100000,
        B00010000,
        B11110100,
        B00111110,
        B01110100,
        B10010000,
        B00100000,
    },
    {
        B00000000,
        B00100000,
        B10010000,
        B01110100,
        B00111110,
        B11110100,
        B00100000,
        B00000000,
    },
},
{
    {
        B00000000,
        B00100000,
        B00010000,
        B11110100,
        B00111110,
        B01110100,
        B10010000,
        B00100000,
    },
    {
        B00000000,
        B00100000,
        B10010000,
        B01110100,
        B00111110,
        B11110100,
        B00100000,
        B00000000,
    },
},
{
    {
        B00000110,
        B00001000,
        B00111110,
        B00110000,
        B00111000,
        B00110110,
        B00111000,
        B00111000,
    },
    {
        B00000000,
        B00000110,
        B00111000,
        B00110000,
        B00111110,
        B00110000,
        B00111000,
        B00111000,
    },
},
{
    {
        B00111000,
        B00111000,
        B00110110,
        B00111000,
        B00110000,
        B00111110,
        B00001000,
        B00000110,
    },
    {
        B00111000,
        B00111000,
        B00110000,
        B00111110,
        B00110000,
        B00111000,
        B00000110,
        B00000000,
    },
},
};

