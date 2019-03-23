/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

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

#include "ssd1306_hal/io.h"

#if defined(CONFIG_USI_SPI_AVAILABLE) && defined(CONFIG_USI_SPI_ENABLE)

#include <stdlib.h>
#include <util/atomic.h>

#define PORT_SPI    PORTB
#define DDR_SPI     DDRB
#define DD_DI       DDB0
#define DD_DO       DDB1
#define DD_SCK      DDB2

UsiSpi::UsiSpi(int8_t csPin, int8_t dcPin)
   : m_cs( csPin )
   , m_dc( dcPin )
{
}

UsiSpi::~UsiSpi()
{
}

void UsiSpi::begin()
{
    if ( m_cs >=0 )
    {
        pinMode( m_cs, OUTPUT );
        digitalWrite( m_cs, HIGH );
    }
    if ( m_dc >= 0) pinMode( m_dc, OUTPUT );
    DDR_SPI |= (1<<DD_DO); // as output (DO) - data out
    DDR_SPI |= (1<<DD_SCK); // as output (USISCK) - clock
    /* DI pin is still used by USI, although ssd1306 library doesn't need it */
//    DDR_SPI &= ~(1<<DD_DI); // as input (DI) - data in
//    PORT_SPI|= (1<<DD_DI); // pullup on (DI)
}

void UsiSpi::end()
{
}

void UsiSpi::start()
{
    if ( m_cs >= 0)
    {
        digitalWrite( m_cs, LOW );
    }
    USICR = (0<<USIWM1) | (1<<USIWM0) |
            (1<<USICS1) | (0<<USICS0) | (1<<USICLK);
}

void UsiSpi::stop()
{
    if ( m_cs >= 0)
    {
        digitalWrite( m_cs, HIGH);
    }
    // TODO:
//    if (ssd1306_lcd.type == LCD_TYPE_PCD8544)
//    {
//        digitalWrite(s_ssd1306_dc, LOW);
//        ssd1306_spiSendByte_Usi( 0x00 ); // Send NOP command to allow last data byte to pass (bug in PCD8544?)
//                                         // ssd1306 E3h is NOP command
//    }
//    NOT TODO:
//    USICR &= ~((1<<USIWM1) | (1<<USIWM0));
}

void UsiSpi::send(uint8_t data)
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

void UsiSpi::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    while (size--)
    {
        USIDR = *buffer;
        USISR = (1<<USIOIF);
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            while ( (USISR & (1<<USIOIF)) == 0 )
            {
                USICR |= (1<<USITC);
            }
        }
        buffer++;
    };
}

#endif


