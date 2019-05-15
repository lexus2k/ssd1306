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

#include "lcd/lcd_common.h"
#include "intf/ssd1306_interface.h"
#include "intf/spi/ssd1306_spi.h"
#include <stddef.h>

#define CMD_ARG 0xFF
#define CMD_DELAY 0xFF

ssd1306_lcd_t ssd1306_lcd = { 0 };

void ssd1306_sendData(uint8_t data)
{
    ssd1306_dataStart();
    ssd1306_lcd.send_pixels1( data );
    ssd1306_intf.stop();
}

void ssd1306_configureI2cDisplay(const uint8_t *config, uint8_t configSize)
{
    ssd1306_commandStart();
    for( uint8_t i=0; i<configSize; i++)
    {
        uint8_t data = pgm_read_byte(&config[i]);
        ssd1306_intf.send(data);
    }
    ssd1306_intf.stop();
}

void ssd1306_configureSpiDisplay(const uint8_t *config, uint8_t configSize)
{
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    for( uint8_t i=0; i<configSize; i++)
    {
        uint8_t data = pgm_read_byte(&config[i]);
        if (data == CMD_ARG)
        {
            data = pgm_read_byte(&config[++i]);
            ssd1306_spiDataMode(1);
            ssd1306_intf.send(data);
            ssd1306_spiDataMode(0);
        }
        else
        {
            ssd1306_intf.send(data);
        }
    }
    ssd1306_intf.stop();
}

void ssd1306_configureSpiDisplay2(const uint8_t *config, uint8_t configSize)
{
    uint8_t command = 1;
    int8_t args;
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    for( uint8_t i=0; i<configSize; i++)
    {
        uint8_t data = pgm_read_byte(&config[i]);
        if ( command )
        {
            if ( command == CMD_DELAY )
            {
                command = 1;
                delay( data == 0xFF ? 500: data );
            }
            else
            {
                ssd1306_intf.send(data);
                command = 0;
                args = -1;
            }
        }
        else
        {
            if (args < 0)
            {
                if ( data >= 128 )
                {
                    command = data;
                }
                else if ( data > 0 )
                {
                    args = data;
                    ssd1306_spiDataMode(1);
                }
                else
                {
                    command = 1;
                }
            }
            else
            {
                args--;
                ssd1306_intf.send(data);
                if ( !args )
                {
                    command = 1;
                    ssd1306_spiDataMode(0);
                }
            }
        }
    }
    ssd1306_intf.stop();
}

void ssd1306_setMode(lcd_mode_t mode)
{
    if (ssd1306_lcd.set_mode)
    {
        ssd1306_lcd.set_mode( mode );
    }
}

void ssd1306_resetController(int8_t rstPin, uint8_t delayMs)
{
    pinMode(rstPin, OUTPUT);
    digitalWrite(rstPin, HIGH);
    /* Wait at least 10ms after VCC is up for LCD */
    delay(10);
    /* Perform reset operation of LCD display */
    digitalWrite(rstPin, LOW);
    delay(delayMs);
    digitalWrite(rstPin, HIGH);
}

