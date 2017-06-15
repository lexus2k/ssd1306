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

const int MAX_LEVELS          = 9;
const int BLOCKS_PER_ROW      = 7;
const int MAX_BLOCKS_PER_ROW  = 8;
const int BLOCK_NUM_ROWS      = 3;


const PROGMEM uint8_t levels[MAX_LEVELS][BLOCK_NUM_ROWS][MAX_BLOCKS_PER_ROW] =
{
    {
        { 1,1,1,1,1,1,1,0 },
        { 0,2,2,2,2,2,0,0 },
        { 0,0,0,0,0,0,0,0 }
    },
    {
        { 0,0,1,1,1,0,0,0 },
        { 0,1,2,2,2,1,0,0 },
        { 0,0,1,1,1,0,0,0 }
    },
    {
        { 2,1,1,1,1,1,2,0 },
        { 0,2,2,5,2,2,0,0 },
        { 2,1,1,1,1,1,2,0 }
    },
    {
        { 2,1,1,3,3,1,2,0 },
        { 1,2,2,2,2,2,1,0 },
        { 0,3,3,3,3,3,0,0 }
    },
    {
        { 0,0,1,0,1,0,0,0 },
        { 0,1,2,6,2,1,0,0 },
        { 0,0,1,0,1,0,0,0 }
    },
    {
        { 0,4,0,4,0,4,0,0 },
        { 4,5,4,5,4,5,4,0 },
        { 0,4,0,4,0,4,0,0 }
    },
    {
        { 2,1,1,5,1,1,2,0 },
        { 2,6,4,1,4,6,2,0 },
        { 2,3,0,0,0,3,2,0 }
    },
    {
        { 0,0,0,0,0,0,0,0 },
        { 0,0,0,0,0,0,0,0 },
        { 0,0,0,0,0,0,0,0 }
    },
    {
        { 2,2,2,2,2,2,2,0 },
        { 2,2,3,1,3,2,2,0 },
        { 3,3,0,0,0,3,3,0 }
    },
};

