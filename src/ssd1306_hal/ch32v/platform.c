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

#if defined(SSD1306_CH32V_PLATFORM)

#include "debug.h"
#include "intf/ssd1306_interface.h"

void delay(uint32_t ms)
{
  Delay_Ms(ms); //
}

#if defined(CONFIG_PLATFORM_I2C_AVAILABLE) && defined(CONFIG_PLATFORM_I2C_ENABLE)
static uint8_t s_i2c_addr = 0x3C;

static void platform_i2c_start(void)
{
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET) {
    ;
  }

  I2C_GenerateSTART(I2C1, ENABLE);

  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
    ;
  }

  I2C_Send7bitAddress(I2C1, s_i2c_addr << 1, I2C_Direction_Transmitter);

  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
    ;
  }
}

static void platform_i2c_stop(void)
{
  // wait for last byte transmitted
  while (!I2C_CheckEvent(I2C1, I2C_STAR1_BTF)) {
    ;
  }
  I2C_GenerateSTOP(I2C1, ENABLE);
}

static void platform_i2c_send(uint8_t data)
{
  // wait for last byte transmitted
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE) == RESET) {
    ;
  }
  I2C_SendData(I2C1, data); // send data byte
}

static void platform_i2c_close(void)
{
  I2C_Cmd(I2C1, DISABLE); //
}

static void platform_i2c_send_buffer(const uint8_t *data, uint16_t len)
{
  for (size_t i = 0; i < len; i++) {
    platform_i2c_send(data[i]);
  }
}

void ssd1306_platform_i2cInit(int8_t busId, uint8_t addr, ssd1306_platform_i2cConfig_t *cfg)
{
  (void)busId; // busId not used, only 1x I2C
  (void)cfg;   // todo, pin number depends on the footprint, a layer arduino-ish in between here is needed, use default.

  if (addr) {
    s_i2c_addr = addr;
  }

  ssd1306_intf.spi         = 0;
  ssd1306_intf.start       = &platform_i2c_start;
  ssd1306_intf.stop        = &platform_i2c_stop;
  ssd1306_intf.send        = &platform_i2c_send;
  ssd1306_intf.close       = &platform_i2c_close;
  ssd1306_intf.send_buffer = &platform_i2c_send_buffer;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure = {
      .GPIO_Pin   = GPIO_Pin_2,
      .GPIO_Mode  = GPIO_Mode_AF_OD,
      .GPIO_Speed = GPIO_Speed_30MHz,
  };
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  I2C_InitTypeDef I2C_InitTSturcture = {
      .I2C_ClockSpeed          = 40000,
      .I2C_Mode                = I2C_Mode_I2C,
      .I2C_DutyCycle           = I2C_DutyCycle_16_9,
      .I2C_Ack                 = I2C_Ack_Enable,
      .I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit,
  };
  I2C_Init(I2C1, &I2C_InitTSturcture);

  I2C_Cmd(I2C1, ENABLE);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////
// TODO
#if defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE)

#include "intf/spi/ssd1306_spi.h"

static void platform_spi_start(void)
{
  // ... Open spi channel for your device with specific s_ssd1306_cs, s_ssd1306_dc
}

static void platform_spi_stop(void)
{
  // ... Complete spi communication
}

static void platform_spi_send(uint8_t data)
{
  // ... Send byte to spi communication channel
}

static void platform_spi_close(void)
{
  // ... free all spi resources here
}

static void platform_spi_send_buffer(const uint8_t *data, uint16_t len)
{
  // ... Send len bytes to spi communication channel here
}

void ssd1306_platform_spiInit(int8_t busId, int8_t cesPin, int8_t dcPin)
{
  if (cesPin >= 0)
    s_ssd1306_cs = cesPin;
  if (dcPin >= 0)
    s_ssd1306_dc = dcPin;
  ssd1306_intf.spi         = 1;
  ssd1306_intf.start       = &platform_spi_start;
  ssd1306_intf.stop        = &platform_spi_stop;
  ssd1306_intf.send        = &platform_spi_send;
  ssd1306_intf.close       = &platform_spi_close;
  ssd1306_intf.send_buffer = &platform_spi_send_buffer;
  // init your interface here
  //...
}
#endif

#endif // YOUR_PLATFORM
