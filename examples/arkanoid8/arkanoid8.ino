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
/**
 *   Nano/Atmega328 PINS: connect LCD to D5 (D/C), D4 (CS), D3 (RES), D11(DIN), D13(CLK)
 *   Attiny SPI PINS:     connect LCD to D4 (D/C), GND (CS), D3 (RES), D1(DIN), D2(CLK)
 *   ESP8266: connect LCD to D1(D/C), D2(CS), RX(RES), D7(DIN), D5(CLK)
 */

#include "ssd1306.h"
#include "ssd1331_api.h"
#include "nano_engine.h"

#include "arkanoid.h"
#include "levels.h"
#include "blocks.h"

NanoEngine8 engine;

uint8_t g_level = 0;

union
{
    struct
    {
        lcdint_t intro_y;
        lcdint_t pauseFrames;
    } intro;
    struct
    {
        NanoRect platform;
        uint8_t  blocks[BLOCK_NUM_ROWS][MAX_BLOCKS_PER_ROW];
        uint8_t  blocksLeft;
        uint8_t  frames;
    } battleField;
} gameState;

void startBattleField(void);

bool drawIntro(void)
{
    engine.canvas.clear();
    engine.canvas.setColor(RGB_COLOR8(255,0,0));
    engine.canvas.drawBitmap1(0, gameState.intro.intro_y, 96, 24, arkanoid_2);
    return true;
}

void introLoop(void)
{
    if (gameState.intro.intro_y < 16)
    {
        gameState.intro.intro_y++;
    }
    else
    {
        gameState.intro.pauseFrames++;
        if (gameState.intro.pauseFrames > engine.getFrameRate() * 3 )
        {
            startBattleField();
        }
    }
    engine.refreshAll();
}

void startIntro(void)
{
    engine.refreshAll();
    g_level = 0;
    gameState.intro.intro_y = -24;
    gameState.intro.pauseFrames = 0;
    engine.drawCallback( drawIntro );
    engine.loopCallback( introLoop );
}


bool drawBattleField(void)
{
    engine.canvas.clear();
    /* If engine requests to redraw main game field */
    if (engine.canvas.offsetx < 64)
    {
        engine.canvas.drawHLine(0,0,63, RGB_COLOR8(255,255,255));
        engine.canvas.drawRect(gameState.battleField.platform, RGB_COLOR8(0,128,255));
        engine.canvas.putPixel(gameState.battleField.platform.left, gameState.battleField.platform.top, 0);
        engine.canvas.putPixel(gameState.battleField.platform.right, gameState.battleField.platform.top, 0);
        if (engine.canvas.offsety < 64)
        {
            for (uint8_t r= 0 /*engine.canvas.offsetx >> 3*/; r<BLOCK_NUM_ROWS; r++)
            {
                for (uint8_t bl = 0; bl<BLOCKS_PER_ROW; bl++)
                {
                    uint8_t block = gameState.battleField.blocks[r][bl];
                    if (block)
                    {
                         engine.canvas.fillRect(bl*8,r*4 + 8,bl*8+8,r*4+4 + 8,blockColors[block]);
                         engine.canvas.drawRect(bl*8,r*4 + 8,bl*8+8,r*4+4 + 8,0);
                    }
                }
            }
         }
    }
    else
    {
        char str[4] = {0};
        engine.canvas.drawVLine(65,0,64, RGB_COLOR8(255,255,255));
        utoa(engine.getCpuLoad( ), str, 10);
        engine.canvas.printFixed(70, 16, str, STYLE_NORMAL );
    }
    return true;
}

void battleFieldLoop(void)
{
    if (engine.pressed( BUTTON_LEFT ) && (gameState.battleField.platform.left > 0))
    {
        engine.refreshRect( gameState.battleField.platform );
        gameState.battleField.platform.shift(-1, 0);
        engine.refreshRect( gameState.battleField.platform );
    }
    if (engine.pressed( BUTTON_RIGHT ) && (gameState.battleField.platform.right < 63))
    {
        engine.refreshRect( gameState.battleField.platform );
        gameState.battleField.platform.shift(+1, 0);
        engine.refreshRect( gameState.battleField.platform );
    }
    gameState.battleField.frames++;
    if ( (gameState.battleField.frames & 0x3F) == 0 )
    {
        engine.refreshRect( 64, 0, 95, 63 );
    }
}

void startBattleField(void)
{
    engine.refreshAll();
    gameState.battleField.platform.setRect( 20, 56, 31, 58 );
    engine.drawCallback( drawBattleField );
    engine.loopCallback( battleFieldLoop );
    /* Loading level */
    if (g_level > MAX_LEVELS)
    {
       g_level = MAX_LEVELS;
    }
    for (uint8_t i =0; i<BLOCKS_PER_ROW; i++)
    {
        for (uint8_t j=0; j<BLOCK_NUM_ROWS; j++)
        {
            gameState.battleField.blocks[j][i] = pgm_read_byte( &levels[g_level][j][i] );
            if ((gameState.battleField.blocks[j][i]) && (gameState.battleField.blocks[j][i] != BLOCK_STRONG))
            {
                gameState.battleField.blocksLeft++;
            }
        }
    }
    gameState.battleField.frames = 0;
}

void setup()
{
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1331_96x64_spi_init(3, 4, 5);

    /* 8-bit engine works only in Horizontal addressing mode */
    ssd1331_setMode(0);

    engine.connectZKeypad(0);

    engine.begin();
    engine.setFrameRate(60);
    startIntro();
}

void loop()
{
    if (!engine.nextFrame()) return;
    engine.display();
}
