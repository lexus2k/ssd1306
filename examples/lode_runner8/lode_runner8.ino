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
/*
 *   Attiny85 PINS
 *             ____
 *   RESET   -|_|  |- 3V
 *   SCL (3) -|    |- (2)
 *   SDA (4) -|    |- (1) - BUZZER
 *   GND     -|____|- (0) - BUTTONS module
 *
 *   Atmega328 PINS with i2c SSD1306 to A4/A5, BUZZER on D8,
 *   Z-keypad ADC module on A0 pin.
 *
 *   Atmega328 PINS with spi Nokia 5110 LCD:
 *   LCD RST to D3
 *   LCD CES to D4
 *   LCD DC to  D5
 *   LCD DIN to D11
 *   LCD CLK to D13
 *   LCD BL to  VCC
 */

#include "ssd1306.h"
#include "nano_engine.h"
#include "sprites.h"

#if defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__)
#define BUZZER      1
#define BUTTON_PIN  0
#else // For Arduino Nano/Atmega328 we use different pins
#define BUZZER      8
#define BUTTON_PIN  0
#endif

uint8_t gameField[16*7] =
{
   5,0,0,0,0,3,3,0,0,0,0,0,0,0,0,5,
   5,0,2,0,4,0,0,2,1,1,2,0,4,0,0,5,
   5,0,2,0,1,1,0,2,0,0,1,1,1,1,0,5,
   5,0,2,0,0,0,0,2,0,4,0,0,0,0,0,5,
   5,1,2,1,1,1,1,1,1,1,1,1,2,1,1,5,
   5,0,2,0,0,0,0,0,4,0,0,0,2,1,0,5,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,5
};

uint8_t blockColors[] = 
{
    RGB_COLOR8(255,96,0),
    RGB_COLOR8(255,255,192),
    RGB_COLOR8(255,255,255),
    RGB_COLOR8(255,255,64),
    RGB_COLOR8(128,128,128),
};

static inline uint8_t blockIdx(uint8_t x, uint8_t y) { return (x>>3) + (((y - 8)>>3) << 4); }
static inline bool isWalkable(uint8_t type)          { return (type == 0) || (type == 2) || (type == 3) || (type == 4); }
static inline bool isSolid(uint8_t type)             { return (type == 1) || (type == 2) || (type == 5); }
static inline bool isPipe(uint8_t type)              { return type == 3; }
static inline bool isGold(uint8_t type)              { return type == 4; }
static inline bool isStair(uint8_t type)             { return type == 2; }

NanoEngine8 engine;

/**
 * Just produces some sound depending on params 
 */
void beep(int bCount,int bDelay);

const uint8_t * playerBitmap;

NanoPoint player = { 8, 8 };

/* The variable is used for player animation      *
 * The graphics defined for the hero has 2 images *
 * for each direction. So, the variable is either *
 * 0 or 1. */
uint8_t  playerAnimation = 0;

/* Timestamp when playerAnimation was changed last time */
uint16_t playerAnimationTs = 0;

/* Number of coins collected */
uint8_t  goldCollection = 0;

void showGameInfo()
{
    for (uint8_t i=0; i<goldCollection; i++)
    {
        engine.canvas.setColor(RGB_COLOR8(255,255,0));
        engine.canvas.drawBitmap1(0 + (i<<3), 0, 8, 8, coinImage);
    }
}

bool onDraw()
{
    engine.canvas.clear();
    engine.canvas.setMode(0);
    for (uint8_t row = 0; row < 7; row++)
    {
        for (uint8_t col = 0; col < 16; col++)
        {
            uint8_t index = (row << 4) + col;
            uint8_t blockType = gameField[index];
            if (blockType != 0)
            {
                engine.canvas.setColor(blockColors[blockType - 1]);
                engine.canvas.drawBitmap1((col<<3), 8 + (row<<3), 8, 8, bgSprites[blockType - 1]);
            }
        }
    }
    showGameInfo();
    engine.canvas.setMode(CANVAS_MODE_TRANSPARENT);
    engine.canvas.setColor(RGB_COLOR8(64,255,255));
    engine.canvas.drawBitmap1(player.x, player.y, 8, 8, playerBitmap);
    return true;
}

void movePlayer(uint8_t direction)
{                                           
    bool animated = false;
    engine.refresh(player.x, player.y, player.x + 7, player.y + 7);
    uint8_t bottomBlock = gameField[blockIdx(player.x + 3,player.y + 8)];
    uint8_t feetBlock = gameField[blockIdx(player.x + 3,player.y+7)];
    uint8_t handBlock = gameField[blockIdx(player.x + 3,player.y)];
    uint8_t centerBlock = gameField[blockIdx(player.x + 3,player.y + 3)];
    uint8_t rightBlock = gameField[blockIdx(player.x + 8,player.y)];
    uint8_t leftBlock = gameField[blockIdx(player.x - 1,player.y)];
    /* If player doesn't stand on the ground, and doesn't hold the pipe,
     * make the player to fall down. */
    if ( !isSolid(bottomBlock) &&
         (!isPipe(handBlock) ||
          !isPipe(feetBlock) ) )
    {
        player.x = (player.x + 3) & ~0x07;
        player.y += 1;
        playerBitmap = &playerFlyingImage[MAN_ANIM_FLYING][playerAnimation][0];
        animated = true;
    }
    else
    {
        switch (direction)
        {
            case BUTTON_RIGHT:
                if (isWalkable(rightBlock))
                {
                    player.x += 1;
                    if (isPipe(centerBlock))
                        playerBitmap = &playerFlyingImage[MAN_ANIM_RIGHT_PIPE][playerAnimation][0];
                    else
                        playerBitmap = &playerFlyingImage[MAN_ANIM_RIGHT][playerAnimation][0];
                    animated = true;
                }
                break;
            case BUTTON_LEFT:
                if (isWalkable(leftBlock))
                {
                    player.x -= 1;
                    if (isPipe(centerBlock))
                        playerBitmap = &playerFlyingImage[MAN_ANIM_LEFT_PIPE][playerAnimation][0];
                    else
                        playerBitmap = &playerFlyingImage[MAN_ANIM_LEFT][playerAnimation][0];
                    animated = true;
                }
                break;
            case BUTTON_UP:
                if (isStair(feetBlock))
                {
                    player.x = (player.x + 3) & ~0x07;
                    player.y -= 1;
                    playerBitmap = &playerFlyingImage[MAN_ANIM_UP][playerAnimation][0];
                    animated = true;
                }
                break;
            case BUTTON_DOWN:
                if ( isStair(bottomBlock) ||
                   (!isSolid(bottomBlock) &&
                     isPipe(handBlock)) )
                {
                    player.x = (player.x + 3) & ~0x07;
                    player.y += 1;
                    playerBitmap = &playerFlyingImage[MAN_ANIM_DOWN][playerAnimation][0];
                    animated = true;
                }
                break;
            default:
                break;
        }
    }
    if (animated && ((uint16_t)(millis() - playerAnimationTs) > 150))
    {
        playerAnimationTs = millis();
        playerAnimation = playerAnimation ? 0 : 1;
        beep(10,20);
        if (isGold(centerBlock))
        {
            engine.notify( "GOLD COIN" );
            gameField[blockIdx(player.x + 3,player.y + 3)] = 0;
            goldCollection++;
            showGameInfo();
            engine.refresh(0,0,63,7);
            /* Produce sound every time the player moves */
            beep(20,40);
            beep(20,80);
            beep(20,120);
            beep(20,80);
            beep(20,40);
        }
    }
    engine.refresh(player.x, player.y, player.x + 7, player.y + 7);
}

void setup()
{
    /* Set font to use in the game. The font has only capital letters and digits */
    ssd1306_setFixedFont(ssd1306xled_font6x8_AB);

    /* Do not init Wire library for Attiny controllers */
//    ssd1306_128x64_i2c_init();

    /* Use the commented lines below, if you want to connect Nokia 5110 LCD */
/*
    pcd8544_84x48_spi_init(3, 4, 5); // 3 RST, 4 CES, 5 DS
*/
    ssd1331_96x64_spi_init(3, 4, 5); // 3 RST, 4 CES, 5 DS
    /* ssd1331 must be initialized in Horizontal addressing mode */
    ssd1331_setMode(0);

    playerBitmap = playerFlyingImage[MAN_ANIM_FLYING][playerAnimation];

    engine.connectZKeypad(BUTTON_PIN);
    engine.drawCallback( onDraw );
    engine.begin();
    engine.setFrameRate(25);
    engine.refresh();
    pinMode(BUZZER, OUTPUT);
}

void loop()
{
    if (!engine.nextFrame()) return;
    movePlayer(engine.buttonsState());
    engine.display();
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
#endif
        }
    }
    digitalWrite(BUZZER,LOW);
}


