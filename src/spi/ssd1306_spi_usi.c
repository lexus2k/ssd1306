/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

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

#include "ssd1306_spi_usi.h"
#include "ssd1306_spi.h"

#include "intf/ssd1306_interface.h"
#include "lcd/lcd_common.h"
#include "hal/io.h"

#ifdef SSD1306_USI_SPI_SUPPORTED

#include <stdlib.h>
#include <util/atomic.h>

#define PORT_SPI    PORTB
#define DDR_SPI     DDRB
#define DD_DI       DDB0
#define DD_DO       DDB1
#define DD_SCK      DDB2

static void ssd1306_spiConfigure_Usi()
{
    DDR_SPI |= (1<<DD_DO); // as output (DO) - data out
    DDR_SPI |= (1<<DD_SCK); // as output (USISCK) - clock
    /* DI pin is still used by USI, although ssd1306 library doesn't need it */
//    DDR_SPI &= ~(1<<DD_DI); // as input (DI) - data in
//    PORT_SPI|= (1<<DD_DI); // pullup on (DI)
}

static void ssd1306_spiClose_Usi()
{
}

static void ssd1306_spiStart_Usi()
{
    if (s_ssd1306_cs >= 0)
    {
        digitalWrite(s_ssd1306_cs,LOW);
    }
    USICR = (0<<USIWM1) | (1<<USIWM0) |
            (1<<USICS1) | (0<<USICS0) | (1<<USICLK);
}

static void ssd1306_spiSendByte_Usi(uint8_t data)
{
    USIDR = data;
    USISR = (1<<USIOIF);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        while ( (USISR & (1<<USIOIF)) == 0 )
        {
            USICR |= (1<<USITC);
        }
    }
}

static void ssd1306_spiStop_Usi()
{
    if (s_ssd1306_cs >= 0)
    {
        digitalWrite(s_ssd1306_cs, HIGH);
    }
    if (g_lcd_type == LCD_TYPE_PCD8544)
    {
        digitalWrite(s_ssd1306_dc, LOW);
        ssd1306_spiSendByte_Usi( 0x00 ); // Send NOP command to allow last data byte to pass (bug in PCD8544?)
                                         // ssd1306 E3h is NOP command
    }
//    USICR &= ~((1<<USIWM1) | (1<<USIWM0));
}

void ssd1306_spiInit_Usi(int8_t cesPin, int8_t dcPin)
{
    if (cesPin >=0)
    {
        pinMode(cesPin, OUTPUT);
        digitalWrite(cesPin, HIGH);
    }
    if (dcPin >= 0) pinMode(dcPin, OUTPUT);
    if ((cesPin >= 0) || (dcPin >= 0))
    {
        /* Even if CS pin is not used we need still to set it to value passed to the function */
        s_ssd1306_cs = cesPin;
        s_ssd1306_dc = dcPin;
    }
    ssd1306_spiConfigure_Usi();
    ssd1306_startTransmission = ssd1306_spiStart_Usi;
    ssd1306_endTransmission = ssd1306_spiStop_Usi;
    ssd1306_sendByte = ssd1306_spiSendByte_Usi;
    ssd1306_closeInterface = ssd1306_spiClose_Usi;
    ssd1306_commandStart = ssd1306_spiCommandStart;
    ssd1306_dataStart = ssd1306_spiDataStart;
}

#endif


