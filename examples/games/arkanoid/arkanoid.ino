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
/*
 * This game is based on original game, developed by Ilya Titov in 2014.
 * It can be still flashed to http://webboggles.com/attiny85-breakout-keychain-game/ HW.
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
 * LEFT (D 4) PD4  6|    |23  PC0  Z-KEYPAD (A 0) if USE_Z_KEYPAD is defined (refer to buttons.h)
 *            VCC  7|    |22  GND
 *            GND  8|    |21  AREF
 * BUZZ (D 6) PB6  9|    |20  AVCC
 *            PB7 10|    |19  PB5
 *            PD5 11|    |18  PB4
 *            PD6 12|    |17  PB3
 *            PD7 13|    |16  PB2
 *            PB0 14|____|15  PB1
 *
 *  IMPORTANT!!! D6 is used instead of D3 with SSD1331 display mode
 */

#include "ssd1306.h"
#include "intf/ssd1306_interface.h"
#include "intf/i2c/ssd1306_i2c.h"
#include "lcd/lcd_common.h"

#include <stdlib.h>

//#define ARKANOID_SSD1331
#if defined(ESP32) || defined(ESP8266) || (defined(__linux__)) || defined(__MINGW32__)
#define DEEP_SLEEP_NOT_SUPPORTED
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
#    define BUZZER      1
#else // For Arduino Nano/Atmega328 we use different pins
#    ifdef ARKANOID_SSD1331
#        define BUZZER      6
#    else
#        define BUZZER      3
#    endif
#endif


const int LEFT_EDGE         = 0;
#ifdef ARKANOID_SSD1331
const int RIGHT_EDGE        = (96 - 14);
const int SCREEN_WIDTH      = (96 - 16);
const uint8_t OUTPUT_OFFSET = 16;
#else
const int RIGHT_EDGE        = (128 - 14);
const int SCREEN_WIDTH      = (128 - 16);
const uint8_t OUTPUT_OFFSET = 0;
#endif
const int SCREEN_HEIGHT     = 64;
const int BLOCK_WIDTH       = 16;
const int PLATFORM_HEIGHT   = 10;
const int INITIAL_PLATFORM_WIDTH = 16;
const int INITIAL_H_SPEED   = -1;
const int INITIAL_V_SPEED   = -4;
const int PLATFORM_SPEED    = 1;
const int MAX_GAME_OBJECTS  = 4;
const int PLATFORM_ROW      = 7;

const uint8_t SPEED_SHIFT  = 2;

uint8_t    platformPos;       // platform position
bool       updateStatusPanel; // Set to true if status panel update is required
static const int  platformWidth = INITIAL_PLATFORM_WIDTH;
int        ballx;
int        bally;
int8_t     vSpeed;
int8_t     hSpeed;
uint8_t    hearts;
uint16_t   lastDrawTimestamp;
uint8_t    gameField[BLOCK_NUM_ROWS][MAX_BLOCKS_PER_ROW];
GameObject objects[MAX_GAME_OBJECTS];
uint16_t    score;
uint8_t    level = 1;
uint8_t    blocksLeft = 0;
uint8_t    platformPower;

void nextLevel();
void beep(int bCount,int bDelay);
void movePlatform();
bool moveObjects();
void drawBall(uint8_t lastx, uint8_t lasty);
bool moveBall();
void drawObjects();
void system_sleep();
void onKill();

static char tempStr[4] = {0};
void arkanoidUtoa(uint16_t b)
{
    utoa(b,tempStr,10);
}


void drawIntro()
{
#ifdef ARKANOID_SSD1331
    ssd1331_96x64_spi_init(3,4,5);
#elif defined(__AVR_ATtiny85__)
    ssd1306_i2cInitEx(-1,-1,0);
#elif defined(CONFIG_SOFTWARE_I2C_AVAILABLE)
    ssd1306_i2cInit_Embedded(-1,-1,0);
#elif defined(CONFIG_PLATFORM_I2C_AVAILABLE)
    ssd1306_platform_i2cInit(-1,0,-1);
#elif defined(CONFIG_TWI_I2C_AVAILABLE)
    ssd1306_i2cInit_Twi(0);
#else
    #error "Not supported microcontroller or board"
#endif
#ifndef ARKANOID_SSD1331
    ssd1306_128x64_init();
#endif
    ssd1306_clearScreen( );
    ssd1306_setColor(RGB_COLOR8(255,0,0));
    for (int8_t y=-24; y<16; y++)
    {
        gfx_drawMonoBitmap(16 - OUTPUT_OFFSET, y, 96, 24, arkanoid_2);
        delay(20);
    }
    ssd1306_setColor(RGB_COLOR8(255,255,0));
    ssd1306_printFixed_oldStyle(40 - OUTPUT_OFFSET, 40, "BREAKOUT", STYLE_NORMAL);
    beep(200,600);
    beep(300,200);
    beep(400,300);
}

void drawStatusPanel()
{
    ssd1306_setColor(RGB_COLOR8(255,0,0));
    for(uint8_t i=0; i<min(hearts,3); i++)
    {
        SPRITE heart = ssd1306_createSprite( RIGHT_EDGE + 4, 16 + (i<<3), 8, heartSprite );
        heart.draw();
    }
    ssd1306_setColor(RGB_COLOR8(255,255,0));
    arkanoidUtoa(score);
    tempStr[2] = '\0';
    ssd1306_printFixed_oldStyle(RIGHT_EDGE + 1, 8, tempStr, STYLE_NORMAL);
    ssd1306_setColor(RGB_COLOR8(0,255,255));
    SPRITE power = ssd1306_createSprite( RIGHT_EDGE + 4, 40, 8, powerSprite );
    if (platformPower)
        power.draw();
    else
        power.erase();
}

/* Draws and clears platform */
void drawPlatform()
{
    uint8_t pos = (platformPos < PLATFORM_SPEED) ? 0: (platformPos - PLATFORM_SPEED);
    ssd1306_setColor(RGB_COLOR8(255,255,0));
    ssd1306_lcd.set_block( pos + LEFT_EDGE + 1, PLATFORM_ROW, platformWidth + PLATFORM_SPEED * 2 );
    while (pos < platformPos)
    {
       ssd1306_lcd.send_pixels1(0B00000000);
       pos++;
    }
    ssd1306_lcd.send_pixels1(0B00001110);
    pos++;
    while (pos < platformPos + platformWidth - 1)
    {
      ssd1306_lcd.send_pixels1(0B00000111);
      pos++;
    }
    ssd1306_lcd.send_pixels1(0B00001110);
    while (pos < platformPos + platformWidth + PLATFORM_SPEED - 1)
    {
       if (pos >= (RIGHT_EDGE - LEFT_EDGE - 2))
       {
          break;
       }
       ssd1306_lcd.send_pixels1(0B00000000);
       pos++;
    }
    ssd1306_intf.stop();
}

void drawFieldEdges()
{
    uint8_t i=8;
    ssd1306_setColor(RGB_COLOR8(255,0,0));
    while (i)
    {
        i--;
        ssd1306_lcd.set_block(LEFT_EDGE, i, 1);
        ssd1306_lcd.send_pixels1( 0B01010101 );
        ssd1306_intf.stop();
        ssd1306_lcd.set_block(RIGHT_EDGE, i, 1);
        ssd1306_lcd.send_pixels1( 0B01010101 );
        ssd1306_intf.stop();
    }
}

void drawBlock(uint8_t x, uint8_t y)
{
    uint8_t block = gameField[y][x];
    switch(block)
    {
        case 1: ssd1306_setColor(RGB_COLOR8(64,64,255)); break;
        case 2: ssd1306_setColor(RGB_COLOR8(64,255,255)); break;
        case 3: ssd1306_setColor(RGB_COLOR8(64,255,64)); break;
        default: ssd1306_setColor(RGB_COLOR8(64,64,255)); break;
    }
    ssd1306_drawSpriteEx(LEFT_EDGE + 1 + (x << 4), y, 16, &blockImages[block][0]);
}


void resetBlocks()
{
    if (level > MAX_LEVELS)
    {
       level = MAX_LEVELS;
    }
    blocksLeft = 0;
    for (uint8_t i =0; i<BLOCKS_PER_ROW; i++)
    {
        for (uint8_t j=0; j<BLOCK_NUM_ROWS; j++)
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

void drawStartScreen()
{
    ssd1306_clearScreen( );
    drawBlocks();
    drawFieldEdges();
    updateStatusPanel = true;
}

void startLevel()
{
    ssd1306_setColor(RGB_COLOR8(255,128,0));
    arkanoidUtoa(level);
    ssd1306_clearScreen();
    ssd1306_printFixed_oldStyle(40 - OUTPUT_OFFSET, 24, "LEVEL ", STYLE_BOLD);
    ssd1306_printFixed_oldStyle(76 - OUTPUT_OFFSET, 24, tempStr, STYLE_BOLD);
    delay(2000);
    resetBlocks();
    hSpeed = INITIAL_H_SPEED;
    vSpeed = INITIAL_V_SPEED;
    platformPos = random(0, (RIGHT_EDGE - LEFT_EDGE - 1 - platformWidth));
    ballx = ( platformPos + ( platformWidth >> 1 ) ) << SPEED_SHIFT;
    bally = ( SCREEN_HEIGHT - PLATFORM_HEIGHT ) << SPEED_SHIFT;
    memset(&objects[0], 0, sizeof(objects));
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

void setup()
{
    ssd1306_setFixedFont_oldStyle(ssd1306xled_font6x8_AB);
    randomSeed(analogRead(0));
#if defined(CONFIG_PLATFORM_I2C_AVAILABLE)
    // no need to do anything here
#elif defined(ARKANOID_SSD1331)
    #ifndef USE_Z_KEYPAD
        pinMode(LEFT_BTN, INPUT);
        pinMode(RIGHT_BTN, INPUT);
    #endif
    pinMode(BUZZER, OUTPUT);
    sei();                      // enable all interrupts
#elif defined(__AVR_ATtiny85__)
    DDRB |= 0b00011010;         // set PB1 as output (for the speaker), PB0 and PB2 as input
    sei();                      // enable all interrupts
#elif defined(CONFIG_ARDUINO_WIRE_LIBRARY_AVAILABLE)
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
#elif defined(CONFIG_SOFTWARE_I2C_AVAILABLE)
    #ifndef USE_Z_KEYPAD
        pinMode(LEFT_BTN, INPUT);
        pinMode(RIGHT_BTN, INPUT);
    #endif
    pinMode(BUZZER, OUTPUT);
    sei();                      // enable all interrupts
#else
    #error "Not supported microcontroller or board"
#endif
    resetGame();
}

void loop()
{
    if ( (uint16_t)(((uint16_t)millis()) - lastDrawTimestamp) > 28 )
    {
        uint8_t lastx = (ballx >> SPEED_SHIFT);
        uint8_t lasty = (bally >> SPEED_SHIFT);
        lastDrawTimestamp += 28;
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
            }
        }
        if (updateStatusPanel)
        {
            updateStatusPanel = false;
            drawStatusPanel();
        }
    }
}

void drawBall(uint8_t lastx, uint8_t lasty)
{
    uint8_t newx = ballx >> SPEED_SHIFT;
    uint8_t newy = bally >> SPEED_SHIFT;
    uint8_t temp;
    temp = 0B00000001 << (newy & 0x07);
    ssd1306_setColor(RGB_COLOR8(255,255,255));
    ssd1306_lcd.set_block(LEFT_EDGE + 1 + newx, newy >> 3, 1);
    ssd1306_lcd.send_pixels1( temp );
    ssd1306_intf.stop();
    if ((newx != lastx) || ((newy >> 3) != (lasty >> 3)))
    {
        ssd1306_lcd.set_block(LEFT_EDGE + 1 + lastx, lasty >> 3, 1);
        ssd1306_lcd.send_pixels1( 0B00000000 );
        ssd1306_intf.stop();
    }
}


void drawObjects()
{
    ssd1306_setColor(RGB_COLOR8(255,0,192));
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
    // Use A0 ADC input (channel 0) if Z_KEYPAD is attached
    uint8_t buttonCode = getPressedButton(0);
    if (buttonCode == BUTTON_RIGHT)
    {
        platformPos = min(RIGHT_EDGE - LEFT_EDGE - 1 - platformWidth, platformPos + PLATFORM_SPEED);
    }
    if (buttonCode == BUTTON_LEFT)
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
    ssd1306_setColor(RGB_COLOR8(255,255,255));
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
    ssd1306_printFixed_oldStyle(32 - OUTPUT_OFFSET, 16, "GAME OVER", STYLE_NORMAL);
    ssd1306_printFixed_oldStyle(32 - OUTPUT_OFFSET, 32, "SCORE ", STYLE_NORMAL);
    arkanoidUtoa(score);
    ssd1306_printFixed_oldStyle(70 - OUTPUT_OFFSET, 32, tempStr, STYLE_NORMAL);
    ssd1306_printFixed_oldStyle(32 - OUTPUT_OFFSET, 40, "TOP SCORE ", STYLE_NORMAL);
    arkanoidUtoa(topScore);
    ssd1306_printFixed_oldStyle(90 - OUTPUT_OFFSET, 40, tempStr, STYLE_NORMAL);
    for (int i = 0; i<1000; i++)
    {
       beep(1,random(0,i*2));
    }
    delay(2000);
}

void platformCrashAnimation()
{
    for (uint8_t j = 4; j > 0; j--)
    {
        for ( uint8_t i = 0; i < platformWidth >> 2; i++ )
        {
            ssd1306_lcd.set_block( platformPos + ((j & 0x01)<<1) + ((j & 0x02)>>1) + (i<<2) + LEFT_EDGE + 1, PLATFORM_ROW, platformWidth );
            ssd1306_lcd.send_pixels1( 0B00000000 );
            ssd1306_intf.stop();
        }
        delay(150);
    }
}


void onKill()
{
    platformCrashAnimation();
    if (hearts == 0)
    {
        // game over if the ball misses the platform
        gameOver();
        system_sleep();
        level = 1;
        resetGame();
    }
    else
    {
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
     vSpeed = -max(4 - abs(hSpeed), 2);
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
    for (int i = bCount*2; i>0; i--)
    {
        digitalWrite(BUZZER, i & 1);
        for(int i2 = 0; i2 < bDelay; i2++)
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

#ifdef DEEP_SLEEP_NOT_SUPPORTED

void system_sleep()
{
}

#else
void system_sleep()
{
  ssd1306_clearScreen( );
  ssd1306_displayOff();
  ADCSRA &= ~(1<<ADEN);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // System actually sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out
  ADCSRA |= (1<<ADEN);
  ssd1306_displayOn();
}
#endif

