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

#include "ssd1306_i2c.h"
#include "intf/ssd1306_interface.h"

void ssd1306_i2cCommandStart(void)
{
    ssd1306_i2cStart();
    ssd1306_i2cSendByte(0x00);
}

void ssd1306_i2cDataStart(void)
{
    ssd1306_i2cStart();
    ssd1306_i2cSendByte(0x40);
}

void ssd1306_i2cInit()
{
    ssd1306_startTransmission = ssd1306_i2cStart;
    ssd1306_endTransmission = ssd1306_i2cStop;
    ssd1306_sendByte = ssd1306_i2cSendByte;
    ssd1306_commandStart = ssd1306_i2cCommandStart;
    ssd1306_dataStart = ssd1306_i2cDataStart;
}
