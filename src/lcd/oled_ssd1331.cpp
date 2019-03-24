/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

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

#include "ssd1306_hal/io.h"
#include "oled_ssd1331.h"

////////////////////////////////////////////////////////////////////////////////
//             SSD1331 basic 8-bit implementation
////////////////////////////////////////////////////////////////////////////////

void DisplaySSD1331_96x64_SPI::begin()
{
    m_spi.begin();
    DisplaySSD1331::begin();
    m_spi.setRotation( 0x00 );
}

void DisplaySSD1331_96x64_SPI::end()
{
    DisplaySSD1331::end();
    m_spi.end();
}

////////////////////////////////////////////////////////////////////////////////
//             SSD1331 basic 16-bit implementation
////////////////////////////////////////////////////////////////////////////////

void DisplaySSD1331_96x64x16_SPI::begin()
{
    m_spi.begin();
    DisplaySSD1331x16::begin();
    m_spi.setRotation( 0x00 );
}

void DisplaySSD1331_96x64x16_SPI::end()
{
    DisplaySSD1331x16::end();
    m_spi.end();
}
