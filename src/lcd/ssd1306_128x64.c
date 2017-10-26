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

#include "ssd1306_128x64.h"
#include "lcd_common.h"
#include "intf/ssd1306_commands.h"
#include "intf/ssd1306_interface.h"
#include "i2c/ssd1306_i2c.h"

static const uint8_t PROGMEM s_oled128x64_initData[] =
{
    SSD1306_DISPLAYOFF, // display off
    /* actually 2 commands below can be omitted */
    SSD1306_SETLOWCOLUMN, 
    SSD1306_SETHIGHCOLUMN, 
    /* normal page addressing mode */
    SSD1306_SETSTARTLINE,
    SSD1306_SETCONTRAST, 0x7F, // contast value
    SSD1306_SEGREMAP | 0x01,
    SSD1306_COMSCANDIR,
    SSD1306_NORMALDISPLAY,
    SSD1306_SETMULTIPLEX, 63,
    SSD1306_SETDISPLAYOFFSET, 0x00, // --no offset
    SSD1306_SETDISPLAYCLOCKDIV, 0x80,
    SSD1306_SETPRECHARGE, 0xF1, // --switch vcc
    SSD1306_SETCOMPINS, 0x12, // --set divide ratio
    SSD1306_SETVCOMDETECT, 0x40, // --vcom detect
    SSD1306_MEMORYMODE, HORIZONTAL_ADDRESSING_MODE, // Page Addressing mode
    SSD1306_CHARGEPUMP, 0x14,
    SSD1306_DISPLAYALLON_RESUME,
    SSD1306_NORMALDISPLAY,
    SSD1306_DISPLAYON 
};


void    ssd1306_init()
{
    ssd1306_128x64_i2c_init();
}


void    ssd1306_128x64_i2c_init()
{
    g_lcd_type = LCD_TYPE_SSD1306;
    ssd1306_i2cInit();
    ssd1306_endTransmission();
    s_displayHeight = 64;
    s_displayWidth = 128;
    for( uint8_t i=0; i<sizeof(s_oled128x64_initData); i++)
    {
        ssd1306_sendCommand(pgm_read_byte(&s_oled128x64_initData[i]));
    }
}

void   ssd1306_128x64_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
{
    if (cesPin >=0) pinMode(cesPin, OUTPUT);
    if (dcPin >= 0) pinMode(dcPin, OUTPUT);
    if (rstPin >=0)
    {
        pinMode(rstPin, OUTPUT);
        digitalWrite(rstPin, HIGH);
        /* Wait at least 1ms after VCC is up for LCD */
        delay(1);
        /* Perform reset operation of LCD display */
        digitalWrite(rstPin, LOW);
        delay(10);
        digitalWrite(rstPin, HIGH);
    }
    g_lcd_type = LCD_TYPE_SSD1306;

    ssd1306_hwSpiInit(cesPin, dcPin);
    ssd1306_commandStart();
    s_displayHeight = 64;
    s_displayWidth = 128;
    for( uint8_t i=0; i<sizeof(s_oled128x64_initData); i++)
    {
        ssd1306_sendCommand(pgm_read_byte(&s_oled128x64_initData[i]));
    }
    ssd1306_endTransmission();
}
