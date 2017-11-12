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
#include "ssd1306_spi_hw.h"
#include "intf/ssd1306_interface.h"
#include "ssd1306_spi_conf.h"
#include "lcd/lcd_common.h"

int8_t s_ssd1306_cs = 4;
int8_t s_ssd1306_dc = 5;

void ssd1306_spiInit(int8_t cesPin, int8_t dcPin)
{
#ifdef SSD1306_SPI_SUPPORTED
    ssd1306_spiConfigure_hw();
    ssd1306_spiInit_hw(cesPin, dcPin);
#endif
}

void ssd1306_spiCommandStart()
{
    digitalWrite(s_ssd1306_dc, LOW);
    ssd1306_startTransmission();
}

void ssd1306_spiDataStart()
{
    digitalWrite(s_ssd1306_dc, HIGH);
    ssd1306_startTransmission();
}


