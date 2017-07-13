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

#include "level_sprites.h"

PROGMEM const uint8_t bgSprites[5][8] =
{
    {
        B01110111,
        B01110111,
        B01110000,
        B01110111,
        B01110111,
        B00000111,
        B01110111,
        B01110111,
    },
    {
        B00010001,
        B11111111,
        B00010001,
        B00010001,
        B00010001,
        B11111111,
        B00010001,
        B00000000,
    },
    {
        B00000010,
        B00000010,
        B00000010,
        B00000010,
        B00000010,
        B00000010,
        B00000010,
        B00000010,
    },
    {
        B00000000,
        B11000000,
        B11100000,
        B10110000,
        B11010000,
        B10100000,
        B11000000,
        B00000000,
    },
    {
        B01111111,
        B01111111,
        B01111111,
        B01110111,
        B01111111,
        B01111111,
        B01111111,
        B00000000,
    },
};

