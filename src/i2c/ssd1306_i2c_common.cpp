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
#include "ssd1306_i2c_wire.h"
#include "ssd1306_i2c_embedded.h"

void ssd1306_i2cCommandStart(void)
{
    ssd1306_startTransmission();
    ssd1306_sendByte(0x00);
}

void ssd1306_i2cDataStart(void)
{
    ssd1306_startTransmission();
    ssd1306_sendByte(0x40);
}

void ssd1306_i2cInit()
{
#ifdef SSD1306_WIRE_SUPPORTED
    ssd1306_i2cInit_Wire(SSD1306_SA);
#elif defined(SSD1306_I2C_SW_SUPPORTED)
    ssd1306_i2cInit_Embedded(SSD1306_SCL, SSD1306_SDA, SSD1306_SA);
#endif
}
