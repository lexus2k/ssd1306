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
//                        ARDUINO I2C IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////

#if defined(CONFIG_ARDUINO_I2C_AVAILABLE) && defined(CONFIG_ARDUINO_I2C_ENABLE)

#include <Wire.h>

static uint8_t s_bytesWritten = 0;

ArduinoI2c::ArduinoI2c(int8_t scl, int8_t sda, uint8_t sa)
    : m_scl( scl )
    , m_sda( sda )
    , m_sa( sa )
{
}

ArduinoI2c::~ArduinoI2c()
{
}

void ArduinoI2c::begin()
{
#if defined(ESP8266) || defined(ESP32) || defined(ESP31B)
    if ((m_scl >= 0) && (m_sda >=0))
    {
        Wire.begin(m_sda, m_scl);
    }
    else
#endif
    {
        Wire.begin();
    }
    #ifdef SSD1306_WIRE_CLOCK_CONFIGURABLE
        Wire.setClock(400000);
    #endif
}

void ArduinoI2c::end()
{
}

void ArduinoI2c::start()
{
    Wire.beginTransmission(m_sa);
    s_bytesWritten = 0;
}

void ArduinoI2c::stop()
{
    Wire.endTransmission();
}

void ArduinoI2c::send(uint8_t data)
{
    // Do not write too many bytes for standard Wire.h. It may become broken
#if defined(ESP32) || defined(ESP31B)
    if (s_bytesWritten >= (I2C_BUFFER_LENGTH >> 4))
#elif defined(ARDUINO_ARCH_SAMD)
    if (s_bytesWritten >= 64)
#elif defined(BUFFER_LENGTH)
    if (s_bytesWritten >= (BUFFER_LENGTH - 2))
#elif defined(SERIAL_BUFFER_LENGTH)
    if (s_bytesWritten >= (SERIAL_BUFFER_LENGTH - 2))
#elif defined(USI_BUF_SIZE)
    if (s_bytesWritten >= (USI_BUF_SIZE -2))
#else
    if ( Wire.write(data) != 0 )
    {
        s_bytesWritten++;
        return;
    }
#endif
    {
        stop();
        start();
        send(0x40);
        /* Commands never require many bytes. Thus assume that user tries to send data */
    }
    Wire.write(data);
    s_bytesWritten++;
}

void ArduinoI2c::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    while (size--)
    {
        send(*buffer);
        buffer++;
    }
}

#endif // CONFIG_PLATFORM_I2C_AVAILABLE

#endif // ARDUINO
