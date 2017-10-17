/*
    Copyright (C) 2016-2017 Alexey Dynda

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


#include "ssd1306_i2c.h"

#include "intf/ssd1306_interface.h"
#include "ssd1306_i2c_conf.h"


/**
 * Port registers, containing pins, which SSD1306 display is connected to.
 * For ATtiny controllers it is standard PORTB
 * For ATmega328p, it is PORTC, which corresponds to Analog inputs/outputs
 */
#if defined(SSD1306_EMBEDDED_I2C)

#include <avr/interrupt.h>

    #if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
        // at 8Mhz each command takes ~ 0.125us
        #define DDR_REG      DDRB
        #define PORT_REG     PORTB
    #else // For Atmega
        // at 16Mhz each command takes ~ 0.0625us
        #define DDR_REG      DDRC
        #define PORT_REG     PORTC
    #endif


#ifndef F_CPU
    #warning "F_CPU is not defined, there can be I2C issues"
    #define F_CPU 8000000
#endif
#define CPU_CYCLE_NS   (1000000000/F_CPU)

// each delay loop takes 4 cycles: nop(1), dec(1), jnz(2) 
#define DELAY_LOOP_CYCLES 4
#define ssd1306_delay(x) for(uint8_t i2=x; i2>0; i2--){__asm__("nop\n\t");}

/**
 * Section, which defines I2C timings for SSD1306 display from datasheet
 */
#define SSD1306_I2C_START_STOP_DELAY 600
#define SSD1306_I2C_RISE_TIME        300
#define SSD1306_I2C_FALL_TIME        300
#define SSD1306_I2C_DATA_HOLD_TIME   300
#define SSD1306_I2C_IDLE_TIME        1300
#define SSD1306_I2C_CLOCK            2500


#define I2C_START_STOP_DELAY ((SSD1306_I2C_START_STOP_DELAY/CPU_CYCLE_NS + DELAY_LOOP_CYCLES/2)/DELAY_LOOP_CYCLES)

#define I2C_RISE_TIME ((SSD1306_I2C_RISE_TIME/CPU_CYCLE_NS)/DELAY_LOOP_CYCLES)

#define I2C_DATA_HOLD_TIME ((SSD1306_I2C_DATA_HOLD_TIME/CPU_CYCLE_NS + DELAY_LOOP_CYCLES/2)/DELAY_LOOP_CYCLES)

#define I2C_IDLE_TIME   (((SSD1306_I2C_IDLE_TIME/CPU_CYCLE_NS) + DELAY_LOOP_CYCLES/2)/DELAY_LOOP_CYCLES)

#define I2C_HALF_CLOCK (((SSD1306_I2C_CLOCK - SSD1306_I2C_FALL_TIME - SSD1306_I2C_RISE_TIME - SSD1306_I2C_FALL_TIME)/CPU_CYCLE_NS/2 \
                         )/DELAY_LOOP_CYCLES)


/* I2C HIGH = PORT as INPUT(0) and PULL-UP ENABLE (1) */
#define DIGITAL_WRITE_HIGH(DREG, PREG, BIT) { DREG &= ~(1 << BIT); PREG |= (1 << BIT); }

/* I2C LOW  = PORT as OUTPUT(1) and OUTPUT LOW (0) */
#define DIGITAL_WRITE_LOW(DREG, PREG, BIT)  { DREG |= (1 << BIT); PREG &= ~(1 << BIT); }

static uint8_t oldSREG;
static bool    interruptsOff = false;

/**
 * SCL remains HIGH on EXIT, Low SDA means start transmission
 */
void ssd1306_i2cStart(void)
{
    oldSREG = SREG;
    cli();
    interruptsOff = true;
    DIGITAL_WRITE_LOW(DDR_REG, PORT_REG, SSD1306_SDA);     // Set to LOW
    ssd1306_delay(I2C_START_STOP_DELAY);
    DIGITAL_WRITE_LOW(DDR_REG, PORT_REG, SSD1306_SCL);     // Set to LOW
    ssd1306_delay(I2C_HALF_CLOCK);
    ssd1306_i2cSendByte((SSD1306_SA << 1) | 0);
}

void ssd1306_i2cStop(void)
{
    DIGITAL_WRITE_LOW(DDR_REG, PORT_REG, SSD1306_SDA);		// Set to LOW
    ssd1306_delay(I2C_RISE_TIME); // Fall time is the same as rise time
    DIGITAL_WRITE_HIGH(DDR_REG, PORT_REG, SSD1306_SCL);	// Set to HIGH
    ssd1306_delay(I2C_START_STOP_DELAY); 
    DIGITAL_WRITE_HIGH(DDR_REG, PORT_REG, SSD1306_SDA);	// Set to HIGH
    ssd1306_delay(I2C_IDLE_TIME);
    if (interruptsOff)
    {
        SREG = oldSREG;
        interruptsOff = false;
    }
}

/**
 * Inputs: SCL is LOW, SDA is has no meaning
 * Outputs: SCL is LOW
 */
void ssd1306_i2cSendByte(uint8_t data)
{
  uint8_t i;
  for(i=0; i<8; i++)
    {
      if((data << i) & 0x80)
        DIGITAL_WRITE_HIGH(DDR_REG, PORT_REG, SSD1306_SDA)
      else
        DIGITAL_WRITE_LOW(DDR_REG, PORT_REG, SSD1306_SDA);
      ssd1306_delay(I2C_RISE_TIME); // Fall time is the same as rise time

      DIGITAL_WRITE_HIGH(DDR_REG, PORT_REG, SSD1306_SCL);
      ssd1306_delay(I2C_HALF_CLOCK);

      DIGITAL_WRITE_LOW(DDR_REG, PORT_REG, SSD1306_SCL);
      ssd1306_delay(I2C_HALF_CLOCK);
    }
  // generating confirmation impulse
  DIGITAL_WRITE_HIGH(DDR_REG, PORT_REG, SSD1306_SDA);
  ssd1306_delay(I2C_RISE_TIME); // Fall time is the same as rise time
  DIGITAL_WRITE_HIGH(DDR_REG, PORT_REG, SSD1306_SCL);
  ssd1306_delay(I2C_HALF_CLOCK);
  DIGITAL_WRITE_LOW(DDR_REG, PORT_REG, SSD1306_SCL); 
  ssd1306_delay(I2C_HALF_CLOCK);
}

#endif


