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

#include "ssd1306_spi_hw.h"
#include "ssd1306_spi.h"

#include "intf/ssd1306_interface.h"
#include "ssd1306_spi_conf.h"
#include "lcd/lcd_common.h"

#ifdef SSD1306_SPI_SUPPORTED
/* STANDARD branch */
    #include <SPI.h>

void ssd1306_spiConfigure_hw()
{
    SPI.begin();
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
    ssd1306_commandStart = ssd1306_spiCommandStart;
    ssd1306_dataStart = ssd1306_spiDataStart;
}

void ssd1306_spiStart_hw()
{
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    if (s_ssd1306_cs >= 0)
    {
        digitalWrite(s_ssd1306_cs,LOW);
    }
}

void ssd1306_spiStop_hw()
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

void ssd1306_spiSendByte_hw(uint8_t data)
{
    SPI.transfer(data);
}

#endif


