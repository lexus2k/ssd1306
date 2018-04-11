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


#include "ssd1306_spi.h"
#include "ssd1306_spi_hw.h"
#include "ssd1306_spi_avr.h"
#include "ssd1306_spi_usi.h"
#include "ssd1306_spi_linux.h"
#include "intf/ssd1306_interface.h"
#include "lcd/lcd_common.h"
#include "hal/io.h"

int8_t s_ssd1306_cs = 4;
int8_t s_ssd1306_dc = 5;
uint32_t s_ssd1306_spi_clock = 8000000;

void ssd1306_spiInit(int8_t cesPin, int8_t dcPin)
{
#if defined(SSD1306_AVR_SPI_SUPPORTED)
    ssd1306_spiInit_avr(cesPin, dcPin);
#elif defined(SSD1306_SPI_SUPPORTED)
    ssd1306_spiConfigure_hw();
    ssd1306_spiInit_hw(cesPin, dcPin);
#elif defined(SSD1306_USI_SPI_SUPPORTED)
    ssd1306_spiInit_Usi(cesPin, dcPin);
#elif defined(SSD1306_LINUX_SUPPORTED)
    /* -1 means SPI bus is selected by default */
    ssd1306_spiInit_Linux(-1, cesPin, dcPin);
#endif
}

void ssd1306_spiDataMode(uint8_t mode)
{
    if (mode)
    {
        digitalWrite(s_ssd1306_dc, HIGH);
    }
    else
    {
        digitalWrite(s_ssd1306_dc, LOW);
    }
}
