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
#include "ssd1306_spi_avr.h"
#include "ssd1306_spi_usi.h"
#include "intf/ssd1306_interface.h"
#include "lcd/lcd_common.h"
#include "ssd1306_hal/io.h"

int8_t s_ssd1306_cs = 4;
int8_t s_ssd1306_dc = 5;
uint32_t s_ssd1306_spi_clock = 8000000;

void ssd1306_spiInit(int8_t cesPin, int8_t dcPin)
{
#if defined(CONFIG_AVR_SPI_AVAILABLE) && defined(CONFIG_AVR_SPI_ENABLE)
    ssd1306_spiInit_avr(cesPin, dcPin);
#elif defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE)
    ssd1306_platform_spiInit(-1, cesPin, dcPin);
#elif defined(CONFIG_USI_SPI_AVAILABLE) && defined(CONFIG_USI_SPI_ENABLE)
    ssd1306_spiInit_Usi(cesPin, dcPin);
#else
    #warning "ssd1306 library: no spi support for the target platform"
#endif
}

void ssd1306_spiDataMode(uint8_t mode)
{
    if (s_ssd1306_dc)
    {
        digitalWrite(s_ssd1306_dc, mode ? HIGH : LOW);
    }
}
