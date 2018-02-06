/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

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

#include "ssd1306_interface.h"

#include "lcd/ssd1306_commands.h"
#include "lcd/lcd_common.h"
#include <stddef.h>

void  (*ssd1306_startTransmission)(void) = NULL;
void  (*ssd1306_endTransmission)(void) = NULL;
void  (*ssd1306_sendByte)(uint8_t data) = NULL;
void  (*ssd1306_commandStart)(void) = NULL;
void  (*ssd1306_dataStart)(void) = NULL;
void  (*ssd1306_closeInterface)(void) = NULL;

void  (*ssd1306_setRamBlock)(uint8_t x, uint8_t y, uint8_t w) = NULL;
void  (*ssd1306_nextRamPage)(void) = NULL;
void  (*ssd1306_sendPixels)(uint8_t data) = NULL;

void ssd1306_sendCommand(uint8_t command)
{
    ssd1306_commandStart();
    ssd1306_sendByte(command);
    ssd1306_endTransmission();
}

void ssd1306_sendData(uint8_t data)
{
    ssd1306_dataStart();
    ssd1306_sendPixels( data );
    ssd1306_endTransmission();
}
