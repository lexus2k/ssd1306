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
#include "intf/ssd1306_interface.h"
#include "intf/spi/ssd1306_spi.h"

typedef NanoEngine<TILE_16x16_RGB8> GameEngine;

#if defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__)
#define BUZZER      1
#define BUTTON_PIN  0
#else // For Arduino Nano/Atmega328 we use different pins
#define BUZZER      8
#define BUTTON_PIN  0
#endif

//#define SSD1331_ACCELERATION

uint8_t gameField[24*7] =
{
   5,0,0,0,0,3,3,0,0,0,0,5,5,0,0,0,0,3,3,0,0,0,0,5,
   5,0,2,0,4,0,0,2,1,1,2,0,0,0,0,0,4,0,0,2,1,1,2,5,
   5,0,2,0,1,1,0,2,0,0,1,1,5,2,0,0,1,1,0,2,0,0,1,1,
   5,0,2,0,0,0,0,2,0,4,0,0,0,2,0,0,0,0,0,2,0,4,0,5,
   5,1,2,1,1,1,1,1,1,1,1,1,5,1,2,1,1,1,1,1,1,1,1,1,
   5,0,2,0,0,0,0,0,4,0,0,5,5,0,2,0,0,0,0,0,4,0,0,5,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

uint8_t blockColors[] = 
{
    RGB_COLOR8(255,96,0),
    RGB_COLOR8(255,255,192),
    RGB_COLOR8(255,255,255),
    RGB_COLOR8(255,255,64),
    RGB_COLOR8(128,128,128),
};

static inline uint8_t blockIdx(uint8_t x, uint8_t y) { return (x>>3) + (((y - 8)>>3) * 24); }
static inline bool isWalkable(uint8_t type)          { return (type == 0) || (type == 2) || (type == 3) || (type == 4); }
static inline bool isSolid(uint8_t type)             { return (type == 1) || (type == 2) || (type == 5); }
static inline bool isPipe(uint8_t type)              { return type == 3; }
static inline bool isGold(uint8_t type)              { return type == 4; }
static inline bool isStair(uint8_t type)             { return type == 2; }

GameEngine engine;

/**
 * Just produces some sound depending on params 
 */
void beep(int bCount,int bDelay);

NanoSprite<GameEngine, engine> player( { 8, 8 }, { 8, 8 }, nullptr );

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
        engine.canvas.drawBitmap1(0 + (i<<3) + engine.offset.x, 0, 8, 8, coinImage);
    }
}

bool onDraw()
{
    engine.canvas.clear();
    engine.canvas.setMode(0);
    for (uint8_t row = max(0,(engine.canvas.offset.y >> 3) - 1); row <= min(6,((engine.canvas.offset.y + engine.NE_TILE_HEIGHT - 1) >> 3) - 1); row++)
    {
        for (uint8_t col = (engine.canvas.offset.x >> 3); col <= min(23, ((engine.canvas.offset.x + engine.NE_TILE_WIDTH - 1) >> 3)); col++)
        {
            uint8_t index = (row * 24) + col;
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
    player.draw();
    return true;
}

void movePlayer(uint8_t direction)
{                                           
    bool animated = false;
    uint8_t bottomBlock = gameField[blockIdx(player.bottom().x,player.bottom().y)];
    uint8_t feetBlock = gameField[blockIdx(player.bottom().x,player.bottom().y - 1)];
    uint8_t handBlock = gameField[blockIdx(player.top().x,player.top().y)];
    uint8_t centerBlock = gameField[blockIdx(player.center().x,player.center().y)];
    uint8_t rightBlock = gameField[blockIdx(player.right().x,player.right().y)];
    uint8_t leftBlock = gameField[blockIdx(player.left().x,player.left().y)];
    /* If player doesn't stand on the ground, and doesn't hold the pipe,
     * make the player to fall down. */
    if ( !isSolid(bottomBlock) &&
         (!isPipe(handBlock) ||
          !isPipe(feetBlock) ) )
    {
        player.moveTo( { player.center().x & ~0x07, player.y() + 1 } );
        player.setBitmap( &playerFlyingImage[MAN_ANIM_FLYING][playerAnimation][0] );
        animated = true;
    }
    else
    {
        switch (direction)
        {
            case BUTTON_RIGHT:
                if (isWalkable(rightBlock))
                {
                    if (player.x() - engine.offset.x >= 64)
                    {
                        int16_t newX = min(player.x() - 64, 96);
                        if (newX != engine.offset.x)
                        {
                            #ifdef SSD1331_ACCELERATION
                            ssd1331_copyBlock((newX - engine.offset.x), 8, 95, 63, 0, 8);
                            engine.offset.x = newX;
                            engine.refresh(95-7, 8, 95, 63);
                            #else
                            engine.offset.x = newX;
                            engine.refresh();
                            #endif
                        }
                    }
                    player.moveBy( { 1, 0 } );
                    if (isPipe(centerBlock))
                        player.setBitmap( &playerFlyingImage[MAN_ANIM_RIGHT_PIPE][playerAnimation][0] );
                    else
                        player.setBitmap( &playerFlyingImage[MAN_ANIM_RIGHT][playerAnimation][0] );
                    animated = true;
                }
                break;
            case BUTTON_LEFT:
                if (isWalkable(leftBlock))
                {
                    if (player.x() - engine.offset.x < 32)
                    {
                        int16_t newX = max(0, player.x() - 32);
                        if (newX != engine.offset.x)
                        {
                            #ifdef SSD1331_ACCELERATION
                            ssd1331_copyBlock(0, 8, 95 - (engine.offset.x - newX), 63, engine.offset.x - newX, 8);
                            engine.offset.x = newX;
                            engine.refresh(0, 8, 7, 63);
                            #else
                            engine.offset.x = newX;
                            engine.refresh();
                            #endif
                        }
                    }
                    player.moveBy( { -1, 0 } );
                    if (isPipe(centerBlock))
                        player.setBitmap( &playerFlyingImage[MAN_ANIM_LEFT_PIPE][playerAnimation][0] );
                    else
                        player.setBitmap( &playerFlyingImage[MAN_ANIM_LEFT][playerAnimation][0] );
                    animated = true;
                }
                break;
            case BUTTON_UP:
                if (isStair(feetBlock))
                {
                    player.moveTo( { player.top().x & ~0x07, player.top().y - 1 } );
                    player.setBitmap( &playerFlyingImage[MAN_ANIM_UP][playerAnimation][0] );
                    animated = true;
                }
                break;
            case BUTTON_DOWN:
                if ( isStair(bottomBlock) ||
                   (!isSolid(bottomBlock) &&
                     isPipe(handBlock)) )
                {
                    player.moveTo( { player.top().x & ~0x07, player.top().y + 1 } );
                    player.setBitmap( &playerFlyingImage[MAN_ANIM_DOWN][playerAnimation][0] );
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
            gameField[blockIdx(player.center().x,player.center().y)] = 0;
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
}

void setup()
{
    /* Set font to use in the game. The font has only capital letters and digits */
    ssd1306_setFixedFont(ssd1306xled_font6x8_AB);

    ssd1331_96x64_spi_init(3, 4, 5); // 3 RST, 4 CES, 5 DS

//    ssd1306_128x64_i2c_init();
//    pcd8544_84x48_spi_init(3, 4, 5); // 3 RST, 4 CES, 5 DS
//    il9163_128x128_spi_init(3, 4, 5);
//    st7735_128x160_spi_init(3, 4, 5);

    player.setBitmap( playerFlyingImage[MAN_ANIM_FLYING][playerAnimation] );

    engine.connectZKeypad(BUTTON_PIN);
    engine.drawCallback( onDraw );
    engine.begin();
    engine.setFrameRate(45);
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