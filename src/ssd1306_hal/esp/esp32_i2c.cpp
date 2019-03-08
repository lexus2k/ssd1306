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

#include "ssd1306_hal/io.h"

#if defined(CONFIG_ESP32_I2C_AVAILABLE) && defined(CONFIG_PLATFORM_I2C_ENABLE)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <stdio.h>
#include "driver/i2c.h"

EspI2c::EspI2c(int8_t busId, uint8_t sa,
               int8_t scl, int8_t sda,
               uint32_t frequency)
    : IWireInterface()
    , m_busId( busId )
    , m_sa( sa )
    , m_scl( scl )
    , m_sda( sda )
{
    if ( m_busId < 0) m_busId = I2C_NUM_1;
    if ( m_sda < 0 ) m_sda = 21;
    if ( m_scl < 0 ) m_scl = 22;
    i2c_config_t conf{};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = static_cast<gpio_num_t>(m_sda);
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = static_cast<gpio_num_t>(m_scl);
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = frequency;
    i2c_param_config( static_cast<i2c_port_t>(m_busId), &conf );
    i2c_driver_install( static_cast<i2c_port_t>(m_busId), conf.mode, 0, 0, 0 );
}

EspI2c::~EspI2c()
{
    i2c_driver_delete( static_cast<i2c_port_t>(m_busId) );
}

void EspI2c::start()
{
    m_cmd_handle = i2c_cmd_link_create();
    i2c_master_start(m_cmd_handle);
    i2c_master_write_byte(m_cmd_handle, ( m_sa << 1 ) | I2C_MASTER_WRITE, 0x1);
}

void EspI2c::stop()
{
    i2c_master_stop( m_cmd_handle );
    i2c_master_cmd_begin( static_cast<i2c_port_t>(m_busId), m_cmd_handle, 1000 / portTICK_RATE_MS );
    i2c_cmd_link_delete(m_cmd_handle);
}

void EspI2c::send(uint8_t data)
{
    i2c_master_write_byte(m_cmd_handle, data, 0x1);
}

void EspI2c::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    while (size--)
    {
        send(*buffer);
        buffer++;
    }
}

#endif

