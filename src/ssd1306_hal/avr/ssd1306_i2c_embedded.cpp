/*
    MIT License

    Copyright (c) 2016-2018, Alexey Dynda

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

#if defined(CONFIG_SOFTWARE_I2C_AVAILABLE) && defined(CONFIG_SOFTWARE_I2C_ENABLE)

/**
 * Port registers, containing pins, which SSD1306 display is connected to.
 * For ATtiny controllers it is standard PORTB
 * For ATmega328p, it is PORTC, which corresponds to Analog inputs/outputs
 */

#ifndef SSD1306_SA
    #define SSD1306_SA    0x3C  // Slave address
#endif

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    #ifndef SSD1306_SCL
        #define SSD1306_SCL   3 ///< SCL, Pin 3 on SSD1306 Board
    #endif
    #ifndef SSD1306_SDA
        #define SSD1306_SDA   4 ///< SDA, Pin 4 on SSD1306 Board
    #endif
#elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    #ifndef SSD1306_SCL
        #define SSD1306_SCL   4 ///< SCL, Pin 4 - physical pin 9 of Attiny84
    #endif
    #ifndef SSD1306_SDA
        #define SSD1306_SDA   6 ///< SDA, Pin 6 - physical pin 7 of Attiny84
    #endif
#else
    #ifndef SSD1306_SCL
        #define SSD1306_SCL   5 // SCL, Pin A5 on SSD1306 Board
    #endif
    #ifndef SSD1306_SDA
        #define SSD1306_SDA   4 // SDA, Pin A4 on SSD1306 Board
    #endif
#endif

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    // at 8Mhz each command takes ~ 0.125us
    #define DDR_REG      DDRB
    #define PORT_REG     PORTB
#elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    // For AttinyX4 controllers
    // at 8Mhz each command takes ~ 0.125us
    #define DDR_REG      DDRA
    #define PORT_REG     PORTA
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
//#define DIGITAL_WRITE_HIGH(DREG, PREG, BIT) { DREG &= ~(1 << BIT); PREG |= (1 << BIT); }
#define DIGITAL_WRITE_HIGH(DREG, PREG, BIT) { DREG &= ~BIT; PREG |= BIT; }

/* I2C LOW  = PORT as OUTPUT(1) and OUTPUT LOW (0) */
//#define DIGITAL_WRITE_LOW(DREG, PREG, BIT)  { DREG |= (1 << BIT); PREG &= ~(1 << BIT); }
#define DIGITAL_WRITE_LOW(DREG, PREG, BIT)  { DREG |= BIT; PREG &= ~BIT; }

static uint8_t oldSREG;
static uint8_t interruptsOff = 0;

SoftwareI2c::SoftwareI2c(int8_t scl, int8_t sda, uint8_t sa)
    : m_scl( scl >= 0 ? scl : (1<<SSD1306_SCL) )
    , m_sda( sda >= 0 ? sda : (1<<SSD1306_SDA) )
    , m_sa ( sa ? : SSD1306_SA )
{
}

SoftwareI2c::~SoftwareI2c()
{
}

void SoftwareI2c::begin()
{
}

void SoftwareI2c::end()
{
}

void SoftwareI2c::start()
{
    oldSREG = SREG;
    cli();
    interruptsOff = 1;
    DIGITAL_WRITE_LOW(DDR_REG, PORT_REG, m_sda);     // Set to LOW
    ssd1306_delay(I2C_START_STOP_DELAY);
    DIGITAL_WRITE_LOW(DDR_REG, PORT_REG, m_scl);     // Set to LOW
    ssd1306_delay(I2C_HALF_CLOCK);
    send( (m_sa << 1) | 0 );
}

void SoftwareI2c::stop()
{
    DIGITAL_WRITE_LOW(DDR_REG, PORT_REG, m_sda);		// Set to LOW
    ssd1306_delay(I2C_RISE_TIME); // Fall time is the same as rise time
    DIGITAL_WRITE_HIGH(DDR_REG, PORT_REG, m_scl);	// Set to HIGH
    ssd1306_delay(I2C_START_STOP_DELAY);
    DIGITAL_WRITE_HIGH(DDR_REG, PORT_REG, m_sda);	// Set to HIGH
    ssd1306_delay(I2C_IDLE_TIME);
    if (interruptsOff)
    {
        SREG = oldSREG;
        interruptsOff = 0;
    }
}

void SoftwareI2c::send(uint8_t data)
{
    uint8_t i;
    for(i=8; i>0; i--)
    {
        if(data & 0x80)
          DIGITAL_WRITE_HIGH(DDR_REG, PORT_REG, m_sda)
        else
          DIGITAL_WRITE_LOW(DDR_REG, PORT_REG, m_sda);
        data<<=1;
        ssd1306_delay(I2C_RISE_TIME); // Fall time is the same as rise time

        DIGITAL_WRITE_HIGH(DDR_REG, PORT_REG, m_scl);
        ssd1306_delay(I2C_HALF_CLOCK);

        DIGITAL_WRITE_LOW(DDR_REG, PORT_REG, m_scl);
        ssd1306_delay(I2C_HALF_CLOCK);
    }
    // generating confirmation impulse
    DIGITAL_WRITE_HIGH(DDR_REG, PORT_REG, m_sda);
    ssd1306_delay(I2C_RISE_TIME); // Fall time is the same as rise time
    DIGITAL_WRITE_HIGH(DDR_REG, PORT_REG, m_scl);
    ssd1306_delay(I2C_HALF_CLOCK);
    DIGITAL_WRITE_LOW(DDR_REG, PORT_REG, m_scl);
    ssd1306_delay(I2C_HALF_CLOCK);
}

void SoftwareI2c::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    while (size--)
    {
        send(*buffer);
        buffer++;
    }
}

#endif
