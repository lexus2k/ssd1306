/*
    MIT License

    Copyright (c) 2017-2019, Alexey Dynda

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

const int MAX_LEVELS          = 5;
#ifdef ARKANOID_SSD1331
const int BLOCKS_PER_ROW      = 5;
#else
const int BLOCKS_PER_ROW      = 7;
#endif
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
        { 0,3,3,3,3,3,0,0 },
        { 0,1,2,2,2,1,0,0 },
        { 0,0,1,1,1,0,0,0 }
    },
    {
        { 2,1,3,1,3,1,2,0 },
        { 0,2,2,5,2,2,0,0 },
        { 2,1,1,1,1,1,2,0 }
    },
    {
        { 2,1,4,3,3,4,2,0 },
        { 1,2,2,2,2,2,1,0 },
        { 0,3,3,3,3,3,0,0 }
    },
    {
        { 0,0,1,0,1,0,0,0 },
        { 0,1,2,6,2,1,0,0 },
        { 0,0,1,5,1,0,0,0 }
    },
#if 0
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
#endif
};

