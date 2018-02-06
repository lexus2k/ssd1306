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

#include "ssd1306_spi_avr.h"
#include "ssd1306_spi.h"

#include "intf/ssd1306_interface.h"
#include "lcd/lcd_common.h"
#include "hal/io.h"

#ifdef SSD1306_AVR_SPI_SUPPORTED

#include <stdlib.h>

#define PORT_SPI    PORTB
#define DDR_SPI     DDRB
#define DD_MISO     DDB4
#define DD_MOSI     DDB3
#define DD_SS       DDB2
#define DD_SCK      DDB5

static void ssd1306_spiConfigure_avr()
{
    DDR_SPI &= ~((1<<DD_MOSI)|(1<<DD_MISO)|(1<<DD_SS)|(1<<DD_SCK));
    /* Define the following pins as output */
    DDR_SPI |= ((1<<DD_MOSI)|(1<<DD_SS)|(1<<DD_SCK));
    SPCR = ((1<<SPE)|               // SPI Enable
            (0<<SPIE)|              // SPI Interupt Enable
            (0<<DORD)|              // Data Order (0:MSB first / 1:LSB first)
            (1<<MSTR)|              // Master/Slave select   
            (0<<SPR1)|(1<<SPR0)|    // SPI Clock Rate
            (0<<CPOL)|              // Clock Polarity (0:SCK low / 1:SCK hi when idle)
            (0<<CPHA));             // Clock Phase (0:leading / 1:trailing edge sampling)
    SPSR = (1<<SPI2X);              // Double Clock Rate
}

static void ssd1306_spiClose_avr()
{
}

static void ssd1306_spiStart_avr()
{
    if (s_ssd1306_cs >= 0)
    {
        digitalWrite(s_ssd1306_cs,LOW);
    }
}

static void ssd1306_spiSendByte_avr(uint8_t data)
{
    SPDR = data;
    while((SPSR & (1<<SPIF))==0);
}

static void ssd1306_spiStop_avr()
{
    if (s_ssd1306_cs >= 0)
    {
        digitalWrite(s_ssd1306_cs, HIGH);
    }
    if (g_lcd_type == LCD_TYPE_PCD8544)
    {
        digitalWrite(s_ssd1306_dc, LOW);
        ssd1306_spiSendByte_avr( 0x00 ); // Send NOP command to allow last data byte to pass (bug in PCD8544?)
                                         // ssd1306 E3h is NOP command
    }
}

void ssd1306_spiInit_avr(int8_t cesPin, int8_t dcPin)
{
    if (cesPin >=0) pinMode(cesPin, OUTPUT);
    if (dcPin >= 0) pinMode(dcPin, OUTPUT);
    if (cesPin) s_ssd1306_cs = cesPin;
    if (dcPin) s_ssd1306_dc = dcPin;
    ssd1306_spiConfigure_avr();
    ssd1306_startTransmission = ssd1306_spiStart_avr;
    ssd1306_endTransmission = ssd1306_spiStop_avr;
    ssd1306_sendByte = ssd1306_spiSendByte_avr;
    ssd1306_closeInterface = ssd1306_spiClose_avr;
    ssd1306_commandStart = ssd1306_spiCommandStart;
    ssd1306_dataStart = ssd1306_spiDataStart;
}

#endif


