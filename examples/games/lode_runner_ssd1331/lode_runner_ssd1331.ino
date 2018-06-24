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

// Uncomment if you have ssd1331 oled display
//#define SSD1331_ACCELERATION

typedef NanoEngine<TILE_16x16_RGB8> GameEngine;

#if defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__)
#define BUZZER      1
#define BUTTON_PIN  0
#else // For Arduino Nano/Atmega328 we use different pins
#define BUZZER      8
#define BUTTON_PIN  0
#endif

const NanoRect game_window = { {0, 8}, {95, 63} };

uint8_t gameField[24*14] =
{
   5,0,0,0,0,3,3,0,0,0,0,5,5,0,0,0,0,3,3,0,0,0,0,5,
   5,0,2,0,4,0,0,2,1,1,2,0,0,0,0,0,4,0,0,2,1,1,2,5,
   5,0,2,0,1,1,0,2,0,0,1,1,5,2,0,0,1,1,0,2,0,0,1,1,
   5,0,2,0,0,0,0,2,0,4,0,0,0,2,0,0,0,0,0,2,0,4,0,5,
   5,1,2,1,1,1,1,1,1,1,1,1,5,1,2,1,1,1,1,1,1,1,1,1,
   5,0,2,0,0,0,0,0,4,0,0,5,5,0,2,0,0,0,0,0,4,0,0,5,
   1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   5,0,0,0,0,3,3,2,0,0,0,5,5,0,0,0,0,3,3,0,0,0,0,5,
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

static inline bool isWalkable(uint8_t type)          { return (type == 0) || (type == 2) || (type == 3) || (type == 4); }
static inline bool isSolid(uint8_t type)             { return (type == 1) || (type == 2) || (type == 5); }
static inline bool isPipe(uint8_t type)              { return type == 3; }
static inline bool isGold(uint8_t type)              { return type == 4; }
static inline bool isStair(uint8_t type)             { return type == 2; }

static inline uint8_t block_at(const NanoPoint& p)
{
    uint16_t index = ((p.x - game_window.p1.x)>>3) +
                     (((p.y - game_window.p1.y)>>3) * 24);
    if (index >= 24*14) index = 0;
    return gameField[index];
}

static inline void set_block_at(const NanoPoint& p, uint8_t v)
{
    uint16_t index = ((p.x - game_window.p1.x)>>3) +
                     (((p.y - game_window.p1.y)>>3) * 24);
    if (index >= 24*14) index = 0;
    gameField[index] = v;
}

GameEngine engine;

/**
 * Just produces some sound depending on params
 */
void beep(int bCount,int bDelay);

NanoFixedSprite<GameEngine, engine> player( game_window.p1 + (NanoPoint){ 8, 8 }, { 8, 8 }, nullptr );

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
    engine.canvas.setColor(RGB_COLOR8(0,0,0));
    engine.canvas.fillRect({{0,0},{95,game_window.p1.y - 1}});
    for (uint8_t i=0; i<goldCollection; i++)
    {
        engine.canvas.setColor(RGB_COLOR8(255,255,0));
        engine.canvas.drawBitmap1(0 + (i<<3), 0, 8, 8, coinImage);
    }
}

static bool onDraw()
{
    engine.canvas.clear();
    engine.canvas.setMode(0);
    if ((engine.canvas.offset.x + engine.NE_TILE_WIDTH - 1 >= game_window.p1.x) &&
       (engine.canvas.offset.y + engine.NE_TILE_HEIGHT - 1 >= game_window.p1.y) &&
       (engine.canvas.offset.x <= game_window.p2.x) &&
       (engine.canvas.offset.y <= game_window.p2.y))
    {
    engine.worldCoordinates();
    for (uint8_t row = max(0,(engine.canvas.offset.y - game_window.p1.y) >> 3);
                 row <= min(13,((engine.canvas.offset.y - game_window.p1.y + engine.NE_TILE_HEIGHT - 1) >> 3)); row++)
    {
        for (uint8_t col = max(0,(engine.canvas.offset.x - game_window.p1.x)>> 3);
                     col <= min(23, ((engine.canvas.offset.x - game_window.p1.x + engine.NE_TILE_WIDTH - 1) >> 3)); col++)
        {
            uint16_t index = (row * 24) + col;
            uint8_t blockType = gameField[index];
            if (blockType != 0)
            {
                engine.canvas.setColor(blockColors[blockType - 1]);
                engine.canvas.drawBitmap1(game_window.p1.x + (col<<3), game_window.p1.y + (row<<3),
                                          8, 8, bgSprites[blockType - 1]);
            }
        }
    }
    engine.canvas.setMode(CANVAS_MODE_TRANSPARENT);
    engine.canvas.setColor(RGB_COLOR8(64,255,255));
    player.draw();
    engine.localCoordinates();
    }
    showGameInfo();
    return true;
}

static NanoPoint calc_new_screen_position()
{
    NanoPoint position = engine.getPosition() + game_window.p1;
    if (player.x() - position.x >= game_window.width() - 24)
    {
        position.x = min(player.x() - (game_window.width() - 24), 96);
    }
    else if (player.x() - position.x < 24)
    {
        position.x = max(0, player.x() - 24);
    }
    if (player.y() - position.y >= game_window.height() - 24)
    {
        position.y = min(player.y() - (game_window.height() - 24), 64);
    }
    else if (player.y() - position.y < 24)
    {
        position.y = max(0, player.y() - 24);
    }
    return position - game_window.p1;
}

#ifdef SSD1331_ACCELERATION
static void moveGameScreen()
{
    NanoPoint position = calc_new_screen_position();
    if (position != engine.getPosition())
    {
        NanoPoint delta = position - engine.getPosition();
        NanoRect block = game_window + delta;
        block.crop(game_window);
        // Copy most part of OLED content via hardware accelerator
        ssd1331_copyBlock(block.p1.x, block.p1.y, block.p2.x, block.p2.y,
                          block.p1.x - delta.x, block.p1.y - delta.y);
        engine.moveTo( position );
        // Now tell the engine to redraw only new areas
        if ( delta.x > 0)
            engine.refresh(game_window.p2.x - delta.x, game_window.p1.y, game_window.p2.x, game_window.p2.y);
        else if ( delta.x < 0 )
            engine.refresh(game_window.p1.x, game_window.p1.y, game_window.p1.x - delta.x, game_window.p2.y);
        if ( delta.y > 0)
            engine.refresh(game_window.p1.x, game_window.p2.y - delta.y, game_window.p2.x, game_window.p2.y);
        else if ( delta.y < 0 )
            engine.refresh(game_window.p1.x, game_window.p1.y, game_window.p2.x, game_window.p1.y - delta.y);
    }
}

#else // NO SSD1331 Acceleration

static void moveGameScreen()
{
    NanoPoint position = calc_new_screen_position();
    if (position != engine.getPosition())
    {
        engine.moveTo( position );
        engine.refresh( game_window );
    }
}

#endif

void movePlayer(uint8_t direction)
{
    bool animated = false;
    uint8_t bottomBlock = block_at(player.bottom());
    uint8_t feetBlock = block_at(player.bottom() + (NanoPoint){0,1});
    uint8_t handBlock = block_at(player.top());
    uint8_t centerBlock = block_at(player.center());
    uint8_t rightBlock = block_at(player.right());
    uint8_t leftBlock = block_at(player.left());
    moveGameScreen();
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
                    player.moveBy( { -1, 0 } );
                    if (isPipe(centerBlock))
                        player.setBitmap( &playerFlyingImage[MAN_ANIM_LEFT_PIPE][playerAnimation][0] );
                    else
                        player.setBitmap( &playerFlyingImage[MAN_ANIM_LEFT][playerAnimation][0] );
                    animated = true;
                }
                break;
            case BUTTON_UP:
                if (isStair(bottomBlock) || isStair(centerBlock))
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
            set_block_at(player.center(), 0);
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
