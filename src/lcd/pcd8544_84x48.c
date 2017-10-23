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

#include "pcd8544_84x48.h"
#include "lcd_common.h"
#include "intf/pcd8544_commands.h"
#include "intf/ssd1306_interface.h"
#include "spi/ssd1306_spi.h"

static const uint8_t PROGMEM s_lcd84x48_initData[] =
{
    PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION, // switch to extented commands
    PCD8544_SETVOP | 0x12,  // Set vop contrast
    PCD8544_SETTEMP,
    PCD8544_SETBIAS | 0x04, // Set bias mode
    PCD8544_FUNCTIONSET, // switch to basic commands
    PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL
};

void    pcd8544_84x48_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
{
    if (cesPin >=0) pinMode(cesPin, OUTPUT);
    if (dcPin >= 0) pinMode(dcPin, OUTPUT);
    if (rstPin >=0)
    {
        pinMode(rstPin, OUTPUT);
        digitalWrite(rstPin, HIGH);
        /* Wait at least 30ms after VCC is up for LCD */
        delay(30);
        /* Perform reset operation of LCD display */
        digitalWrite(rstPin, LOW);
        delay(1);
        digitalWrite(rstPin, HIGH);
    }
    g_lcd_type = LCD_TYPE_PCD8544;

    ssd1306_hwSpiInit(cesPin, dcPin);
    ssd1306_commandStart();
    s_displayHeight = 48;
    s_displayWidth = 84;
    for( uint8_t i=0; i<sizeof(s_lcd84x48_initData); i++)
    {
        ssd1306_sendByte(pgm_read_byte(&s_lcd84x48_initData[i]));
    }
    ssd1306_endTransmission();
}
