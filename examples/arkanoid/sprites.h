/*
    MIT License

    Copyright (c) 2016-2018, Alexey Dynda

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

const PROGMEM uint8_t heartSprite[8] =
{
    0B00001110,
    0B00011111,
    0B00111111,
    0B01111110,
    0B01111110,
    0B00111101,
    0B00011001,
    0B00001110
};


const PROGMEM uint8_t powerSprite[8] =
{
    0B11001110,
    0B11111110,
    0B00111110,
    0B00010110,
    0B00001110,
    0B00001110,
    0B00001111,
    0B00001110
};


const PROGMEM uint8_t shootSprite[1] =
{
    0B00111100,
};

const PROGMEM uint8_t bonusSprites[][5] =
{
    { /* BLOCK_BOMB */
        0B01111001,
        0B11111110,
        0B11101110,
        0B01111001,
        0B00000000,
    },
    { /* BLOCK_HEART */
        0B00011100,
        0B00111110,
        0B01111100,
        0B00111010,
        0B00011100,
    },
    { /* BLOCK_POWER */
        0B00000000,
        0B01111110,
        0B00010010,
        0B00010010,
        0B00001100,
    },
};


