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
        0B00000000,
        0B00000000,
        0B00000000,
        0B00000000,
        0B00000000,
        0B00000000,
        0B00000000,
        0B00000000,
        0B00000000,
        0B00000000,
        0B00000000,
        0B00000000,
        0B00000000,
        0B00000000,
        0B00000000,
        0B00000000
    },
    {   /* SIMPLE 1 */
        0B00000000,
        0B00111100,
        0B01111110,
        0B01100110,
        0B01100110,
        0B01100110,
        0B01100110,
        0B01100110,
        0B01100110,
        0B01100110,
        0B01100110,
        0B01100110,
        0B01100110,
        0B01111110,
        0B00111100,
        0B00000000
    },
    {   /* DASHED 2 */
        0B00000000,
        0B00111100,
        0B01010010,
        0B01001010,
        0B01100110,
        0B01010010,
        0B01001010,
        0B01100110,
        0B01010010,
        0B01001010,
        0B01100110,
        0B01010010,
        0B01001010,
        0B01100110,
        0B00111100,
        0B00000000
    },  
    {   /* STRONG 3 */
        0B00000000,
        0B00111100,
        0B01111110,
        0B01111110,
        0B01111110,
        0B01111110,
        0B01111110,
        0B01111110,
        0B01111110,
        0B01111110,
        0B01111110,
        0B01111110,
        0B01111110,
        0B01111110,
        0B00111100,
        0B00000000
    },
    {   /* BOMB 4 */
        0B00000000,
        0B00111100,
        0B01111110,
        0B01111110,
        0B01011010,
        0B01011010,
        0B01100110,
        0B01100110,
        0B01011010,
        0B01011010,
        0B01111110,
        0B01111110,
        0B01111010,
        0B01110110,
        0B00111100,
        0B00000000
    },
    {   /* HEART 5 */
        0B00000000,
        0B00111100,
        0B01111110,
        0B01111110,
        0B01111110,
        0B01110110,
        0B01100010,
        0B01000110,
        0B01100010,
        0B01110110,
        0B01111110,
        0B01111110,
        0B01111110,
        0B01111110,
        0B00111100,
        0B00000000
    },  
    {   /* POWER 6 */
        0B00000000,
        0B00111100,
        0B01111110,
        0B01111110,
        0B01000010,
        0B01101010,
        0B01101010,
        0B01101010,
        0B01101010,
        0B01110110,
        0B01111110,
        0B01111110,
        0B01111010,
        0B01110110,
        0B00111100,
        0B00000000
    },
};

