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

#include "ssd1306_spi_hw.h"
#include "ssd1306_spi.h"

#include "intf/ssd1306_interface.h"
#include "lcd/lcd_common.h"
#include "hal/io.h"

#include <stdlib.h>

#ifdef SSD1306_SPI_SUPPORTED
/* STANDARD branch */
    #include <SPI.h>

void ssd1306_spiConfigure_hw()
{
    SPI.begin();
}

static void ssd1306_spiClose_hw()
{
}

static void ssd1306_spiStart_hw()
{
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    if (s_ssd1306_cs >= 0)
    {
        digitalWrite(s_ssd1306_cs,LOW);
    }
}

static void ssd1306_spiStop_hw()
{
    if (s_ssd1306_cs >= 0)
    {
        digitalWrite(s_ssd1306_cs, HIGH);
    }
    if (g_lcd_type == LCD_TYPE_PCD8544)
    {
        digitalWrite(s_ssd1306_dc, LOW);
        SPI.transfer( 0x00 ); // Send NOP command to allow last data byte to pass (bug in PCD8544?)
                              // ssd1306 E3h is NOP command
    }
    SPI.endTransaction();
}

static void ssd1306_spiSendByte_hw(uint8_t data)
{
    SPI.transfer(data);
}

void ssd1306_spiInit_hw(int8_t cesPin, int8_t dcPin)
{
    if (cesPin >=0) pinMode(cesPin, OUTPUT);
    if (dcPin >= 0) pinMode(dcPin, OUTPUT);
    if (cesPin) s_ssd1306_cs = cesPin;
    if (dcPin) s_ssd1306_dc = dcPin;
    ssd1306_startTransmission = ssd1306_spiStart_hw;
    ssd1306_endTransmission = ssd1306_spiStop_hw;
    ssd1306_sendByte = ssd1306_spiSendByte_hw;
    ssd1306_closeInterface = ssd1306_spiClose_hw;
    ssd1306_commandStart = ssd1306_spiCommandStart;
    ssd1306_dataStart = ssd1306_spiDataStart;
}

#endif


