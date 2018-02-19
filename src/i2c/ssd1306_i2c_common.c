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

#include "ssd1306_i2c.h"
#include "intf/ssd1306_interface.h"
#include "ssd1306_i2c_wire.h"
#include "ssd1306_i2c_embedded.h"
#include "ssd1306_i2c_twi.h"
#include "ssd1306_i2c_linux.h"

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
#if defined(SSD1306_WIRE_SUPPORTED)
    ssd1306_i2cConfigure_Wire(scl, sda);
    ssd1306_i2cInit_Wire(sa);
#elif defined(SSD1306_TWI_SUPPORTED)
    ssd1306_i2cConfigure_Twi(0);
    ssd1306_i2cInit_Twi(sa);
#elif defined(SSD1306_I2C_SW_SUPPORTED)
    ssd1306_i2cInit_Embedded(scl, sda, sa);
#elif defined(SSD1306_LINUX_SUPPORTED) && !defined(__KERNEL__)
    ssd1306_i2cInit_Linux(scl, sa);
#endif
}

void ssd1306_i2cInit()
{
    ssd1306_i2cInitEx(-1, -1, SSD1306_SA);
}
