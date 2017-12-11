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
/**
 * Original game was developed by Ilya Titov in 2014. This version is deeply
 * reworked and based on new ssd1306 library.
 */
/*
 *   Attiny85 PINS
 *             ____
 *   RESET   -|_|  |- 3V
 *   SCL (3) -|    |- (2) LEFT
 *   SDA (4) -|    |- (1) BUZZER
 *   GND     -|____|- (0) RIGHT
 *   
 *   
 * ATMEL ATMEGA8 & 168 & 328 / ARDUINO NANO
 *
 *                   ____
 *    10kOm   PC6  1|_|  |28  PC5           
 *            PD0  2|    |27  PC4           
 *            PD1  3|    |26  PC3                  
 * RIGHT(D 2) PD2  4|    |25  PC2                   
 * BUZZ (D 3) PD3  5|    |24  PC1               
 * LEFT (D 4) PD4  6|    |23  PC0  Z-KEYPAD (A 0) if USE_Z_KEYPAD is defined (see below)
 *            VCC  7|    |22  GND
 *            GND  8|    |21  AREF
 *            PB6  9|    |20  AVCC
 *            PB7 10|    |19  PB5            
 *            PD5 11|    |18  PB4             
 *            PD6 12|    |17  PB3            
 *            PD7 13|    |16  PB2            
 *            PB0 14|____|15  PB1            
 * 
 */

#include "ssd1306.h"
#include "i2c/ssd1306_i2c_wire.h"
#include "i2c/ssd1306_i2c_embedded.h"
#include "intf/ssd1306_interface.h"

#if defined(ESP8266) || defined(ESP32)
    #include <pgmspace.h>
#else
    #include <avr/pgmspace.h>
    #include <avr/eeprom.h>
    #include <avr/sleep.h>
    #include <avr/interrupt.h> // needed for the additional interrupt
#endif

#ifdef SSD1306_WIRE_SUPPORTED
#include <Wire.h>
#endif

#include "levels.h"
#include "blocks.h"
#include "sprites.h"
#include "arkanoid.h"
#include "buttons.h"

typedef struct
{
    SPRITE  sprite;
    uint8_t type;
    uint8_t extra;
} GameObject;


uint16_t *EEPROM_ADDR = (uint16_t*)0;

#if defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__)
#    define LEFT_BTN    2
#    define BUZZER      1
#    define RIGHT_BTN   0
#else // For Arduino Nano/Atmega328 we use different pins
#    define USE_Z_KEYPAD // use analog Z-keypad ADC module on A0 pin.
#    ifndef USE_Z_KEYPAD
#        define LEFT_BTN    4
#        define RIGHT_BTN   2
#    endif
#    define BUZZER      3
#endif


const int LEFT_EDGE         = 0;
const int RIGHT_EDGE        = (128 - 14);
const int SCREEN_WIDTH      = (128 - 16);
const int SCREEN_HEIGHT     = 64;
const int BLOCK_WIDTH       = 16;
const int PLATFORM_HEIGHT   = 10;
const int INITIAL_PLATFORM_WIDTH = 16;
const int INITIAL_H_SPEED   = -1;
const int INITIAL_V_SPEED   = -4;
const int PLATFORM_SPEED    = 2;
const int MAX_GAME_OBJECTS  = 4;
const int PLATFORM_ROW      = 7;

const uint8_t SPEED_SHIFT  = 2;

uint8_t    platformPos;       // platform position
bool       updateStatusPanel; // Set to true if status panel update is required
const int  platformWidth = INITIAL_PLATFORM_WIDTH;
int        ballx;
int        bally;
int8_t     vSpeed;
int8_t     hSpeed;
uint8_t    hearts;
uint16_t   lastDrawTimestamp;
uint8_t    gameField[BLOCK_NUM_ROWS][MAX_BLOCKS_PER_ROW];
GameObject objects[MAX_GAME_OBJECTS];
uint16_t   score;
uint8_t    level = 1;
uint8_t    blocksLeft = 0;
uint8_t    platformPower;


void resetGame();
void nextLevel();
void drawPlatform();
void beep(int bCount,int bDelay);
void movePlatform();
bool moveObjects();
void drawBall(uint8_t lastx, uint8_t lasty);
bool moveBall();
void drawBlocks();
void drawObjects();
void system_sleep();
void resetBlocks();
void drawFieldEdges();
void drawStatusPanel();
void onKill();

void playerInc()
{
   // PB2 pin button interrupt
}

void setup()
{
    randomSeed(analogRead(0));
#if defined(__AVR_ATtiny85__)
    DDRB |= 0b00011010;         // set PB1 as output (for the speaker), PB0 and PB2 as input
    sei();                      // enable all interrupts
    attachInterrupt(0,playerInc,HIGH);
#elif defined(SSD1306_WIRE_SUPPORTED)
    Wire.begin();
    #ifdef SSD1306_WIRE_CLOCK_CONFIGURABLE
        Wire.setClock( 400000 );
    #endif
    #ifndef USE_Z_KEYPAD
        pinMode(LEFT_BTN, INPUT);
        pinMode(RIGHT_BTN, INPUT);
    #endif
    pinMode(BUZZER, OUTPUT);
    sei();                      // enable all interrupts
    #ifndef USE_Z_KEYPAD
        attachInterrupt(digitalPinToInterrupt(RIGHT_BTN),playerInc,HIGH);
    #endif
#elif defined(SSD1306_I2C_SW_SUPPORTED)
    #error "Not supported microcontroller or board"
#else
    #error "Not supported microcontroller or board"
#endif
    resetGame();
}

void loop()
{
    if ( ((uint16_t)millis()) - lastDrawTimestamp > 30 )
    {
        uint8_t lastx = (ballx >> SPEED_SHIFT);
        uint8_t lasty = (bally >> SPEED_SHIFT);
        // continue moving after the interrupt
        movePlatform();
        // bounce off the sides of the screen
        if (moveBall())
        {
            if (moveObjects())
            {
               // update whats on the screen
               drawPlatform();
               drawBall(lastx, lasty);
               drawObjects();
               if (updateStatusPanel)
               {
                   updateStatusPanel = false;
                   drawStatusPanel();
               }
               lastDrawTimestamp += 30;
            }
        }
    }
}

void drawStatusPanel()
{
    for(uint8_t i=0; i<min(hearts,3); i++)
    {
        SPRITE heart = ssd1306_createSprite( RIGHT_EDGE + 4, 16 + (i<<3), 8, heartSprite );
        heart.draw();
    }
    char temp[6] = {'0',0,0,0,0,0};
    utoa(score,temp + (score<10?1:0),10);
    temp[2] = '\0';
    ssd1306_charF6x8(RIGHT_EDGE + 1, 1, temp);
    SPRITE power = ssd1306_createSprite( RIGHT_EDGE + 4, 40, 8, powerSprite );
    if (platformPower)
        power.draw();
    else
        power.erase();
}

void drawIntro()
{
#if defined(__AVR_ATtiny85__)
    ssd1306_i2cInit_Embedded(0,0,0);
#elif defined(SSD1306_WIRE_SUPPORTED)
    ssd1306_i2cInit_Wire(0);
#elif defined(SSD1306_I2C_SW_SUPPORTED)
    ssd1306_i2cInit_Embedded(0,0,0);
#else
    #error "Not supported microcontroller or board"
#endif
    ssd1306_128x64_init();
    ssd1306_clearScreen( );
    ssd1306_drawBitmap(16, 2, 96, 24, arkanoid_2);
    ssd1306_charF6x8(40, 5, "BREAKOUT");
    beep(200,600);
    beep(300,200);
    beep(400,300);
}

void drawStartScreen()
{
  ssd1306_clearScreen( );
  drawBlocks();
  drawFieldEdges();
  drawStatusPanel();
}

void startLevel()
{
    resetBlocks();
    hSpeed = INITIAL_H_SPEED;
    vSpeed = INITIAL_V_SPEED;
    platformPos = random(0, (RIGHT_EDGE - LEFT_EDGE - 1 - platformWidth));
    ballx = ( platformPos + ( platformWidth >> 1 ) ) << SPEED_SHIFT;
    bally = ( SCREEN_HEIGHT - PLATFORM_HEIGHT ) << SPEED_SHIFT;
    for(uint8_t i=0; i<MAX_GAME_OBJECTS; i++)
    {
        objects[i].type = 0;
    }
    drawStartScreen();
    lastDrawTimestamp = millis();
}

void resetGame()
{
    score = 0;
//    platformWidth = INITIAL_PLATFORM_WIDTH;
    platformPower = 0;
    hearts = 2;
    drawIntro();
    delay(3000);
    startLevel();
}

/* Draws and clears platform */
void drawPlatform()
{
  uint8_t pos = (platformPos < PLATFORM_SPEED) ? 0: (platformPos - PLATFORM_SPEED);
  ssd1306_setRamBlock( pos + LEFT_EDGE + 1, PLATFORM_ROW, platformWidth + PLATFORM_SPEED * 2 );
  ssd1306_dataStart();
  while (pos < platformPos)
  {
     ssd1306_sendByte(B00000000);
     pos++;
  }
  ssd1306_sendByte(B00001110);
  pos++;
  while (pos < platformPos + platformWidth - 1)
  {
    ssd1306_sendByte(B00000111);
    pos++;
  }
  ssd1306_sendByte(B00001110);
  while (pos < platformPos + platformWidth + PLATFORM_SPEED - 1)
  {
     if (pos >= (RIGHT_EDGE - LEFT_EDGE - 2))
     {
        break;
     }
     ssd1306_sendByte(B00000000);
     pos++;
  }
  ssd1306_endTransmission();
}


void drawBlock(uint8_t x, uint8_t y)
{
    ssd1306_drawSpriteEx(LEFT_EDGE + 1 + (x << 4), y, 16, &blockImages[gameField[y][x]][0]);
}


void resetBlocks()
{
    if (level > MAX_LEVELS)
    {
       level = MAX_LEVELS;
    }
    blocksLeft = 0;
    for (byte i =0; i<BLOCKS_PER_ROW; i++)
    {
        for (int j=0; j<BLOCK_NUM_ROWS; j++)
        {
            gameField[j][i] = pgm_read_byte( &levels[level-1][j][i] );
            if ((gameField[j][i]) && (gameField[j][i] != BLOCK_STRONG))
            {
                blocksLeft++;
            }
        }
    }
}

void drawBlocks()
{
    for (uint8_t r=0; r<BLOCK_NUM_ROWS; r++)
    {
        for (uint8_t bl = 0; bl<BLOCKS_PER_ROW; bl++)
        {
            drawBlock(bl, r);
        }
    }
}

void drawFieldEdges()
{
    for (uint8_t i=8; i>0; i--)
    {
        ssd1306_setRamBlock(LEFT_EDGE, i, 1);
        ssd1306_sendData( B01010101 );
        ssd1306_setRamBlock(RIGHT_EDGE, i, 1);
        ssd1306_sendData( B01010101 );
    }
}


void drawBall(uint8_t lastx, uint8_t lasty)
{
    uint8_t newx = ballx >> SPEED_SHIFT;
    uint8_t newy = bally >> SPEED_SHIFT;
    ssd1306_setRamBlock(LEFT_EDGE + 1 + newx,newy >> 3, 1);
    uint8_t temp = B00000001;
    temp = temp << ((newy & 0x07) + 1);
    ssd1306_sendData( temp );
    if ((newx != lastx) || ((newy >> 3) != (lasty >> 3)))
    {
        ssd1306_setRamBlock(LEFT_EDGE + 1 + lastx, lasty >> 3, 1);
        ssd1306_sendData( B00000000 );
    }
}


void drawObjects()
{
    for(uint8_t i=0; i<MAX_GAME_OBJECTS; i++)
    {
       if (objects[i].type == 0)
       {
       }
       else if (objects[i].type == 1)
       {
           objects[i].sprite.erase();
           objects[i].type = 0;
       }
       else
       {
           objects[i].sprite.eraseTrace();
           objects[i].sprite.draw();
       }
    }
}

uint8_t freeObject()
{
    for(uint8_t i=0; i<MAX_GAME_OBJECTS; i++)
    {
        if (objects[i].type == 0) return i;
    }
    return 0xFF;
}


bool platformHit(uint8_t x, uint8_t y)
{
    if (y >= (SCREEN_HEIGHT - PLATFORM_HEIGHT))
    {
       if ((x >= platformPos) && (x <= platformPos + platformWidth))
       {
           return true;
       }
    }
    return false;
}


enum
{
    BLOCK_HIT_NONE,
    BLOCK_HIT_UNBREAKABLE,
    BLOCK_HIT_BREAKABLE,
    BLOCK_HIT_LEVEL_DONE,
};

uint8_t blockHit(uint8_t x, uint8_t y)
{
    uint8_t ball_row = y>>3;
    if (ball_row < BLOCK_NUM_ROWS)
    {
        uint8_t ball_col = x >> 4;
        uint8_t blockType = gameField[ball_row][ball_col];
        if ( blockType > 0 )
        {
            if (blockType != BLOCK_STRONG)
            {
                gameField[ball_row][ball_col] = 0;
                score++;
                blocksLeft--;
                drawBlock(ball_col, ball_row);
                updateStatusPanel = true;
                if (blockType >= BLOCK_BONUS)
                {
                    uint8_t i = freeObject();
                    if (i != 0xFF)
                    {
                        objects[i].sprite = ssd1306_createSprite( (ball_col << 4) + 6,
                                                                  (ball_row << 3),
                                                                   5,
                                                                   &bonusSprites[blockType - BLOCK_BONUS][0] );
                        objects[i].extra = 0;
                        objects[i].type = blockType;
                    }
                }
            }
            // reset blocks if all have been hit
            if (blocksLeft == 0)
            {
                level++;
                startLevel();
                return BLOCK_HIT_LEVEL_DONE;
            }
            return (blockType == BLOCK_STRONG ? BLOCK_HIT_UNBREAKABLE : BLOCK_HIT_BREAKABLE);
        }
    }
    return BLOCK_HIT_NONE;
}


bool moveObjects()
{
    for(uint8_t i=0; i<MAX_GAME_OBJECTS; i++)
    {
       if (objects[i].type <= 1)
       {
       }
       else if (objects[i].type < BLOCKS_MAX)
       {
           if (objects[i].sprite.y >= (SCREEN_HEIGHT - PLATFORM_HEIGHT - 4))
           {
               objects[i].type = 1;
           }
           else
           {
               if (objects[i].extra-- == 0)
               {
                   objects[i].extra = 1;
                   if (platformHit(objects[i].sprite.x + 3, objects[i].sprite.y + 8))
                   {
                       if (objects[i].type == BLOCK_BOMB)
                       {
                           onKill();
                           return false;
                       }
                       else if (objects[i].type == BLOCK_HEART)
                       {
                           hearts++;
                           updateStatusPanel = true;
                       }
                       else if (objects[i].type == BLOCK_POWER)
                       {
                           platformPower = 255;
                           updateStatusPanel = true;
                       }
                       objects[i].type = 1;
                   }
                   objects[i].sprite.y++;
               }
           }
       }
       else if (objects[i].type == 0xFF)
       {
           if (objects[i].sprite.y <= 1)
           {
               objects[i].type = 1;
           }
           else
           {
               uint8_t hitType = blockHit( objects[i].sprite.x, objects[i].sprite.y - 1 );
               if (hitType != BLOCK_HIT_NONE)
               {
                   if (hitType == BLOCK_HIT_LEVEL_DONE)
                   {
                       return false;
                   }
                   objects[i].type = 1;
               }
               else
               {
                   objects[i].sprite.y -= 1;
               }
           }
       }
    }
    return true;
}


// continues moving after interrupt
void movePlatform()
{
#ifdef USE_Z_KEYPAD
    // Use A0 ADC input (channel 0)
    uint8_t buttonCode = getPressedButton(0);
    if (buttonCode == BUTTON_RIGHT)
#else
    if (digitalRead(RIGHT_BTN) != LOW)
#endif
    {
        platformPos = min(RIGHT_EDGE - LEFT_EDGE - 1 - platformWidth, platformPos + PLATFORM_SPEED);
    }
#ifdef USE_Z_KEYPAD    
    if (buttonCode == BUTTON_LEFT)
#else
    if (digitalRead(LEFT_BTN) != LOW)
#endif
    {
        platformPos = max(0, platformPos - PLATFORM_SPEED);
    }
    if (platformPower  != 0)
    {
        platformPower--;
        if (!(platformPower & 0x1F))
        {
            uint8_t i = freeObject();
            if (i != 0xFF)
            {
                objects[i].sprite = ssd1306_createSprite( platformPos + (platformWidth >> 1),
                                                          SCREEN_HEIGHT - PLATFORM_HEIGHT - 8,
                                                           1,
                                                           shootSprite );
                objects[i].extra = 0;
                objects[i].type = 0xFF;
            }
        }
        if (platformPower == 0) updateStatusPanel = true;
    }
}


void gameOver()
{
#if defined(ESP32) || defined(ESP8266)
    uint16_t topScore = score;
#else
    uint16_t topScore = eeprom_read_word(EEPROM_ADDR);
    if (topScore == 0xFFFF)
    {
        eeprom_write_word(EEPROM_ADDR, 0);
        topScore = 0;
    }
    if (score>topScore)
    {
        topScore = score;
        eeprom_write_word(EEPROM_ADDR, topScore);
    }
#endif
    ssd1306_clearScreen( );
    ssd1306_charF6x8(32, 2, "GAME OVER");
    ssd1306_charF6x8(32, 4, "score ");
    char temp[6] = {0,0,0,0,0,0};
    utoa(score,temp,10);
    ssd1306_charF6x8(70, 4, temp);
    ssd1306_charF6x8(32, 5, "top score ");
    utoa(topScore,temp,10);
    ssd1306_charF6x8(90, 5, temp);
    for (int i = 0; i<1000; i++)
    {
       beep(1,random(0,i*2));
    }
    delay(2000);
}

void platformCrashAnimation()
{
    for (uint8_t j = 0; j < 4; j++)
    {
        for ( uint8_t i = 0; i < platformWidth >> 2; i++ )
        {
            ssd1306_setRamBlock( platformPos + (i<<2) + ((j & 0x01)<<1) + ((j & 0x02)>>1) + LEFT_EDGE + 1, PLATFORM_ROW, platformWidth );
            ssd1306_sendData(B00000000);
        }
        delay(150);
    }
}


void onKill()
{
    if (hearts == 0)
    {
        platformCrashAnimation();
        // game over if the ball misses the platform
        gameOver();
        system_sleep();
        level = 1;
        resetGame();
    }
    else
    {
        platformCrashAnimation();
        hearts--;
        startLevel();
    }
}


// the collsision check is actually done before this is called, this code works
// out where the ball will bounce
void collision(uint8_t partx, uint8_t party)
{
  /* botton / top collision */
  if ((party <= 0) || (party >= 7))
  {
      vSpeed = -vSpeed;
  }
  else if ((partx <= 0) || (partx >= 15))
  {
      hSpeed = -hSpeed;
  }
  beep(30,300);
}


// move and bounce the ball when reaches the screen limits
bool moveBall()
{
  uint8_t nextx = (ballx + hSpeed) >> SPEED_SHIFT;
  uint8_t nexty = (bally + vSpeed) >> SPEED_SHIFT;
  /* checkplatform Hit */
  if (platformHit(nextx, nexty))
  {
     int middle = platformPos + (platformWidth >> 1);
     hSpeed = (nextx - middle) / (platformWidth >> (SPEED_SHIFT + 1));
     vSpeed = -max(4 - abs(hSpeed), 1);
     beep(20,600);
  }
  /* Check screen hit */
  nextx = (ballx + hSpeed) >> SPEED_SHIFT;
  nexty = (bally + vSpeed) >> SPEED_SHIFT;
  if ((nextx <= 0) || (nextx >= RIGHT_EDGE - LEFT_EDGE - 1))
  {
      hSpeed = -hSpeed;
  }
  if (nexty <= 0)
  {
      vSpeed = -vSpeed;
  }
  /* Check game over */
  if (nexty >=(SCREEN_HEIGHT - PLATFORM_HEIGHT + 2))
  {
      onKill();
      return false;
  }
  ballx += hSpeed;
  bally += vSpeed;
  /* Check bar hit */
  uint8_t hitType = blockHit( ballx >> SPEED_SHIFT, bally >> SPEED_SHIFT );
  if (hitType != BLOCK_HIT_NONE)
  {
      if (hitType == BLOCK_HIT_LEVEL_DONE)
      {
          return false;
      }
      if (hitType == BLOCK_HIT_UNBREAKABLE)
      {
          ballx -= hSpeed;
          bally -= vSpeed;
      }
      uint8_t partx = (ballx >> SPEED_SHIFT) & 0x0F;
      uint8_t party = (bally >> SPEED_SHIFT) & 0x07;
      collision(partx, party);
  }
  return true;
}


void beep(int bCount,int bDelay)
{
    for (int i = 0; i<=bCount*2; i++)
    {
        digitalWrite(BUZZER,i&1);
        for(int i2=0; i2<bDelay; i2++)
        {
            __asm__("nop\n\t");
#if F_CPU > 8000000
            __asm__("nop\n\t");
            __asm__("nop\n\t");
            __asm__("nop\n\t");
            __asm__("nop\n\t");
            __asm__("nop\n\t");
            __asm__("nop\n\t");
            __asm__("nop\n\t");
            __asm__("nop\n\t");
#endif
        }
    }
    digitalWrite(BUZZER,LOW);
}

// Routines to set and clear bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#if defined(ESP32) || defined(ESP8266)

void system_sleep()
{
}

#else
void system_sleep()
{
  ssd1306_clearScreen( );
  ssd1306_displayOff();
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // System actually sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
  ssd1306_displayOn();
}
#endif