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

void ssd1306_i2cCommandStart()
{
    ssd1306_startTransmission();
    ssd1306_sendByte(0x00);
}

void ssd1306_i2cDataStart()
{
    ssd1306_startTransmission();
    ssd1306_sendByte(0x40);
}

void ssd1306_i2cInitEx(int8_t scl, int8_t sda, int8_t sa)
{
#ifdef SSD1306_WIRE_SUPPORTED
    ssd1306_i2cConfigure_Wire(scl, sda);
    ssd1306_i2cInit_Wire(sa);
#elif defined(SSD1306_I2C_SW_SUPPORTED)
    ssd1306_i2cInit_Embedded(scl, sda, sa);
#endif
}

void ssd1306_i2cInit()
{
    ssd1306_i2cInitEx(-1, -1, SSD1306_SA);
}
