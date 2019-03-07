/*
    MIT License

    Copyright (c) 2016-2019, Alexey Dynda

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

#if defined(ARDUINO)

//////////////////////////////////////////////////////////////////////////////////
//                        ARDUINO SPI IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE)

/* STANDARD branch */
#include <SPI.h>

ArduinoSpi::ArduinoSpi(int8_t csPin, int8_t dcPin, uint32_t frequency)
    : m_cs( csPin )
    , m_dc( dcPin )
    , m_frequency( frequency )
{
    if (csPin >=0) pinMode(csPin, OUTPUT);
    if (dcPin >= 0) pinMode(dcPin, OUTPUT);
    SPI.begin();
}

ArduinoSpi::~ArduinoSpi()
{
    SPI.end();
}

void ArduinoSpi::start()
{
    /* anyway, oled ssd1331 cannot work faster, clock cycle should be > 150ns: *
     * 1s / 150ns ~ 6.7MHz                                                     */
    SPI.beginTransaction(SPISettings(m_frequency, MSBFIRST, SPI_MODE0));
    if (m_cs >= 0)
    {
        digitalWrite(m_cs,LOW);
    }
}

void ArduinoSpi::stop()
{
    // TODO: PCD8854
//    if (ssd1306_lcd.type == LCD_TYPE_PCD8544)
//    {
//        digitalWrite(s_ssd1306_dc, LOW);
//        SPI.transfer( 0x00 ); // Send NOP command to allow last data byte to pass (bug in PCD8544?)
//                              // ssd1306 E3h is NOP command
//    }
    if (m_cs >= 0)
    {
        digitalWrite(m_cs, HIGH);
    }
    SPI.endTransaction();
}

void ArduinoSpi::send(uint8_t data)
{
    SPI.transfer(data);
}

void ArduinoSpi::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    /* Do not use SPI.transfer(buffer, size)! this method corrupts buffer content */
    while (size--)
    {
        SPI.transfer(*buffer);
        buffer++;
    };
}

#endif // CONFIG_PLATFORM_SPI_AVAILABLE

#endif // ARDUINO
