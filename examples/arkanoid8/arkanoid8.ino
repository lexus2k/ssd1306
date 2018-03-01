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

static uint8_t g_level = 0;
static const NanoRect gameArea = { {16, 0}, {79, 63} };
static const NanoRect blockArea = { {gameArea.p1.x, 8},
                                    {gameArea.p1.x + BLOCKS_PER_ROW*8 - 1, 8 + BLOCK_NUM_ROWS*4 - 1} };

union
{
    struct
    {
        lcdint_t intro_y;
        lcdint_t pauseFrames;
    } intro;
    struct
    {
        NanoRect  platform;
        NanoPoint ball;
        NanoPoint ballSpeed;
        uint8_t   blocks[BLOCK_NUM_ROWS][MAX_BLOCKS_PER_ROW];
        uint8_t   blocksLeft;
        uint8_t   frames;
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
        engine.refreshRect( 0, gameState.intro.intro_y, 95, gameState.intro.intro_y + 24 );
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
    /* If engine requests to redraw main game field */
    if (gameArea.has(engine.canvas.offset))
    {
        engine.canvas.setMode(0);
        engine.canvas.setColor(RGB_COLOR8(0,0,64));
        engine.canvas.drawBitmap1(engine.canvas.offset.x, engine.canvas.offset.y, 8, 8, bgTile);
        engine.canvas.drawHLine(gameArea.p1.x,gameArea.p1.y,gameArea.p2.x, RGB_COLOR8(255,255,255));
        engine.canvas.drawRect(gameState.battleField.platform, RGB_COLOR8(0,128,255));
        engine.canvas.putPixel(gameState.battleField.platform.p1, 0);
        engine.canvas.putPixel(gameState.battleField.platform.p2.x,
                               gameState.battleField.platform.p1.y, 0);
        if (gameArea.has(engine.canvas.offset))
        {
            for (uint8_t r = 0; r<BLOCK_NUM_ROWS; r++)
            {
                for (uint8_t bl = 0; bl<BLOCKS_PER_ROW; bl++)
                {
                    uint8_t block = gameState.battleField.blocks[r][bl];
                    if (block)
                    {
                         NanoRect rect;
                         rect.setRect(bl*8 + gameArea.p1.x, r*4 + 8, bl*8 + gameArea.p1.x + 8, r*4+4 + 8);
                         engine.canvas.fillRect(rect,blockColors[block]);
                         engine.canvas.drawRect(rect,0);
                    }
                }
            }
         }
         engine.canvas.putPixel(gameState.battleField.ball, 0xFF);
    }
    else
    {
        char str[4] = {0};
        engine.canvas.clear();
        engine.canvas.drawVLine(gameArea.p1.x-1,0,64, RGB_COLOR8(255,255,255));
        engine.canvas.drawVLine(gameArea.p2.x+1,0,64, RGB_COLOR8(255,255,255));
        utoa(engine.getCpuLoad( ), str, 10);
        engine.canvas.setColor(RGB_COLOR8(192,192,192));
        engine.canvas.printFixed(gameArea.p2.x+3, 16, str, STYLE_NORMAL );
    }
    return true;
}

bool checkBlockHit(void)
{
    if (!blockArea.has(gameState.battleField.ball))
    {
        return false;
    }
    NanoPoint p = gameState.battleField.ball - blockArea.p1;
    uint8_t row = p.y >> 2;
    uint8_t column = p.x >> 3;
    if (!gameState.battleField.blocks[row][column])
    {
        return false;
    }
    gameState.battleField.blocks[row][column] = 0;
    if (((p.y & 3) == 2) || (p.y & 3) == 1)
    {
        gameState.battleField.ballSpeed.x = -gameState.battleField.ballSpeed.x;
    }
    else
    {
        gameState.battleField.ballSpeed.y = -gameState.battleField.ballSpeed.y;
    }
    engine.refreshPoint( gameState.battleField.ball );
    return true;
}

void battleFieldLoop(void)
{
    if (engine.pressed( BUTTON_LEFT ) && (gameState.battleField.platform.p1.x > gameArea.p1.x))
    {
        engine.refreshRect( gameState.battleField.platform );
        gameState.battleField.platform.add(-1, 0);
        engine.refreshRect( gameState.battleField.platform );
    }
    if (engine.pressed( BUTTON_RIGHT ) && (gameState.battleField.platform.p2.x < gameArea.p2.x))
    {
        engine.refreshRect( gameState.battleField.platform );
        gameState.battleField.platform.add(+1, 0);
        engine.refreshRect( gameState.battleField.platform );
    }
    gameState.battleField.frames++;
    if ( (gameState.battleField.frames & 0x3F) == 0 )
    {
        engine.refreshRect( gameArea.p2.x + 1, 0, 95, 63 );
    }
    engine.refreshPoint( gameState.battleField.ball );
    
    bool moveBall;
    do
    {
        NanoPoint speed = gameState.battleField.ballSpeed;
        moveBall = false;
        gameState.battleField.ball.add( speed );
        if (!gameArea.hasX(gameState.battleField.ball.x))
        {
            moveBall = true;
            gameState.battleField.ballSpeed.x = -gameState.battleField.ballSpeed.x;
        }
        if (!gameArea.hasY(gameState.battleField.ball.y))
        {
            moveBall = true;
            gameState.battleField.ballSpeed.y = -gameState.battleField.ballSpeed.y;
        }
        if (gameState.battleField.platform.hasX( gameState.battleField.ball.x ) && 
            !gameState.battleField.platform.above( gameState.battleField.ball ) &&
            !gameState.battleField.platform.below( gameState.battleField.ball ) )
        {
            moveBall = true;
            gameState.battleField.ballSpeed.y = -gameState.battleField.ballSpeed.y;
        }
        if (checkBlockHit())
        {
            moveBall = true;
        }
    }
    while (moveBall);
    engine.refreshPoint( gameState.battleField.ball );
}

void loadLevel(void)
{
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

void startBattleField(void)
{
    engine.refreshAll();
    gameState.battleField.platform.setRect( 20, 56, 31, 58 );
    gameState.battleField.ball.setPoint( 25, 55);
    gameState.battleField.ballSpeed.setPoint( 1, -1);
    engine.drawCallback( drawBattleField );
    engine.loopCallback( battleFieldLoop );
    loadLevel();
}

void setup()
{
    /* Set font to use in the game. The font has only capital letters and digits */
    ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
    /* Init SPI 96x64 RBG oled. 3 - RESET, 4 - CS (can be omitted, oled CS must be pulled down), 5 - D/C */
    ssd1331_96x64_spi_init(3, 4, 5);

    /* 8-bit engine works only in Horizontal addressing mode */
    ssd1331_setMode(0);

    /* Configure engine to use ZKeypand on A0 as control board. */
    engine.connectZKeypad(0);

    /* Start engine */
    engine.begin();

    /* Set frame rate. 30 fps is too slow */
    engine.setFrameRate(45);

    startIntro();
}

void loop()
{
    if (!engine.nextFrame()) return;
    engine.display();
}
