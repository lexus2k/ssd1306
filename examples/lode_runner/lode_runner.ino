/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

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
#include "game_field.h"
#include "player_sprites.h"
#include "coin_sprite.h"
#include "hero_states.h"
#include "buttons.h"

#if defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__)
#define BUZZER      1
#define BUTTON_PIN  0
#else // For Arduino Nano/Atmega328 we use different pins
#define BUZZER      8
#define BUTTON_PIN  0
#endif

/**
 * Just produces some sound depending on params 
 */
void beep(int bCount,int bDelay);

/* Define player sprite */
SPRITE   playerSprite;

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
        ssd1306_drawBitmap(0 + (i<<3),0,8,8,coinImage);
    }
}

void movePlayer(SPRITE &playerSprite, uint8_t direction)
{
    uint8_t x = playerSprite.x + 3;
    bool animated = false;
    /* If player doesn't stand on the ground, and doesn't hold the pipe,
     * make the player to fall down. */
    if ( !isSolid(gameField[blockIdx(x,playerSprite.y + 8)]) &&
         (!isPipe(gameField[blockIdx(x,playerSprite.y)]) ||
          !isPipe(gameField[blockIdx(x,playerSprite.y + 6)]) ) )
    {
        playerSprite.x = (((x) >> 3) << 3);
        playerSprite.y += 1;
        playerSprite.data = &playerFlyingImage[MAN_ANIM_FLYING][playerAnimation][0];
        animated = true;
    }
    else
    {
        switch (direction)
        {
            case BUTTON_RIGHT:
                if (isWalkable(gameField[blockIdx(playerSprite.x + 8,playerSprite.y)]))
                {
                    playerSprite.x += 1;
                    if (isPipe(gameField[blockIdx(playerSprite.x + 3,playerSprite.y)]))
                    {
                        playerSprite.data = &playerFlyingImage[MAN_ANIM_RIGHT_PIPE][playerAnimation][0];
                    }
                    else
                    {
                        playerSprite.data = &playerFlyingImage[MAN_ANIM_RIGHT][playerAnimation][0];
                    }
                    animated = true;
                }
                break;
            case BUTTON_LEFT:
                if (isWalkable(gameField[blockIdx(playerSprite.x - 1,playerSprite.y)]))
                {
                    playerSprite.x -= 1;
                    if (isPipe(gameField[blockIdx(playerSprite.x + 3,playerSprite.y)]))
                    {
                        playerSprite.data = &playerFlyingImage[MAN_ANIM_LEFT_PIPE][playerAnimation][0];
                    }
                    else
                    {
                        playerSprite.data = &playerFlyingImage[MAN_ANIM_LEFT][playerAnimation][0];
                    }
                    animated = true;
                }
                break;
            case BUTTON_UP:
                if (isStair(gameField[blockIdx(x,playerSprite.y+7)]))
                {
                    playerSprite.x = (((x) >> 3) << 3);
                    playerSprite.y -= 1;
                    playerSprite.data = &playerFlyingImage[MAN_ANIM_UP][playerAnimation][0];
                    animated = true;
                }
                break;
            case BUTTON_DOWN:
                if ( isStair(gameField[blockIdx(x,playerSprite.y+8)]) ||
                   (!isSolid(gameField[blockIdx(x,playerSprite.y+8)]) &&
                     isPipe(gameField[blockIdx(x,playerSprite.y)])) )
                {
                    playerSprite.x = (((x) >> 3) << 3);
                    playerSprite.y += 1;
                    playerSprite.data = &playerFlyingImage[MAN_ANIM_DOWN][playerAnimation][0];
                    animated = true;
                }
                break;
            default:
                break;
        }
    }
    if (animated && ((uint16_t)millis() - playerAnimationTs > 150))
    {
        playerAnimationTs = millis();
        playerAnimation = playerAnimation ? 0 : 1;
        beep(10,20);
        if (isGold(gameField[blockIdx(x,playerSprite.y + 3)]))
        {
            gameField[blockIdx(x,playerSprite.y + 3)] = 0;
            goldCollection++;
            showGameInfo();
            /* Produce sound every time the player moves */
            beep(20,40);
            beep(20,80);
            beep(20,120);
            beep(20,80);
            beep(20,40);
        }
    }
}

static uint32_t lastTs;

void setup()
{
    /* Do not init Wire library for Attiny controllers */
    ssd1306_128x64_i2c_init();

    /* Use the commented lines below, if you want to connect Nokia 5110 LCD */
/*
    pcd8544_84x48_spi_init(3, 4, 5); // 3 RST, 4 CES, 5 DS
*/
    ssd1306_fillScreen(0x00);
    /* Set range of the gameField on the screen in blocks. */
    field.setRect( (SSD1306_RECT) { 0, 1, (ssd1306_displayWidth()>>3) - 1, (ssd1306_displayHeight()>>3) - 1 } );
    playerSprite = ssd1306_createSprite( 8, 8, 8, playerFlyingImage[MAN_ANIM_FLYING][playerAnimation] );
    field.add( playerSprite );
    field.refreshScreen();
    showGameInfo();
    pinMode(BUZZER, OUTPUT);
    lastTs = millis();
}

void loop()
{
    /* Move sprite every 40 milliseconds */
    if ((unsigned long)(millis() - lastTs) >= 40)
    {
        lastTs += 40;
        uint8_t direction = getPressedButton(BUTTON_PIN);
        movePlayer(playerSprite, direction);
        field.drawSprites();
    }
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


