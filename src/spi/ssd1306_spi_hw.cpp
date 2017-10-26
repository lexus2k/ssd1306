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


#include "ssd1306_spi.h"

#include "intf/ssd1306_interface.h"
#include "ssd1306_spi_conf.h"
#include "lcd/lcd_common.h"

#if !defined(SSD1306_EMBEDDED_SPI)
/* STANDARD branch */
    #include <SPI.h>

static int8_t s_cs = 4;
static int8_t s_dc = 5;

void ssd1306_hwSpiInit(int8_t cesPin, int8_t dcPin)
{
    ssd1306_startTransmission = ssd1306_spiStart;
    ssd1306_endTransmission = ssd1306_spiStop;
    ssd1306_sendByte = ssd1306_spiSendByte;
    ssd1306_commandStart = ssd1306_spiCommandStart;
    ssd1306_dataStart = ssd1306_spiDataStart;
    s_cs = cesPin;
    s_dc = dcPin;
}

void ssd1306_spiStart(void)
{
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    if (s_cs >= 0)
    {
        digitalWrite(s_cs,LOW);
    }
}

void ssd1306_spiStop(void)
{
    if (s_cs >= 0)
    {
        digitalWrite(s_cs, HIGH);
    }
    if (g_lcd_type == LCD_TYPE_PCD8544)
    {
        digitalWrite(s_dc, LOW);
        SPI.transfer( 0x00 ); // Send NOP command to allow last data byte to pass (bug in PCD8544?)
                              // ssd1306 E3h is NOP command
    }
    SPI.endTransaction();
}

void ssd1306_spiSendByte(uint8_t data)
{
    SPI.transfer(data);
}

void ssd1306_spiCommandStart()
{
    digitalWrite(s_dc, LOW);
    ssd1306_spiStart();
}

void ssd1306_spiDataStart()
{
    digitalWrite(s_dc, HIGH);
    ssd1306_spiStart();
}

#endif


