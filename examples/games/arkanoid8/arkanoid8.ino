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
#include "nano_engine.h"

#include "arkanoid.h"
#include "levels.h"

#define PIX_BITS  2

NanoEngine8 engine;

static uint8_t g_level = 0;
static uint8_t g_score = 0;
static const NanoRect gameArea = { {16, 0}, {79, 63} };
static const NanoRect blockArea = { {0, 8},
                                    {BLOCKS_PER_ROW*8 - 1, 8 + BLOCK_NUM_ROWS*4 - 1} };

union
{
    struct
    {
        lcdint_t intro_y;
        lcdint_t pauseFrames;
    } intro;
    struct
    {
        lcdint_t intro_y;
        lcdint_t pauseFrames;
    } info;
    struct
    {
        NanoRect  platform;  // platform position on the screen
        NanoPoint ball;      // ball position on the screen
        NanoPoint ballScaled;// ball position in ^PIX_BITS coordinates
        NanoPoint ballSpeed; // ball speed in *2 coordinates
        uint8_t   blocks[BLOCK_NUM_ROWS][MAX_BLOCKS_PER_ROW];
        uint8_t   blocksLeft;
        uint8_t   frames;
    } battleField;
} gameState;

void startBattleField(bool reload);

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
        engine.refresh( 0, gameState.intro.intro_y, 95, gameState.intro.intro_y + 24 );
        gameState.intro.intro_y++;
    }
    else
    {
        gameState.intro.pauseFrames++;
        if (gameState.intro.pauseFrames > engine.getFrameRate() * 3 )
        {
            startBattleField( true );
        }
    }
}

void startIntro(void)
{
    engine.refresh();
    g_level = 0;
    g_score = 0;
    gameState.intro.intro_y = -24;
    gameState.intro.pauseFrames = 0;
    engine.drawCallback( drawIntro );
    engine.loopCallback( introLoop );
}

bool drawBattleField(void)
{
    /* If engine requests to redraw main game field */
    if (gameArea.contains(engine.canvas.rect()))
    {
        /* Set non-transparent mode */
        engine.canvas.setMode(CANVAS_MODE_BASIC);
        engine.canvas.setColor(RGB_COLOR8(0,0,64));
        NanoRect tileBlocks = engine.canvas.rect() >> 3;
        tileBlocks.crop(gameArea >> 3);
        for (uint8_t row = tileBlocks.p1.y; row <= tileBlocks.p2.y; row++)
            for (uint8_t col = tileBlocks.p1.x; col <= tileBlocks.p2.x; col++)
                engine.canvas.drawBitmap1(col << 3, row << 3, 8, 8, bgTile);
        engine.canvas.setColor(RGB_COLOR8(255,255,255));
        engine.canvas.drawHLine(gameArea.p1.x,gameArea.p1.y,gameArea.p2.x);

        /* Now draw everything in game coordinates */
        engine.worldCoordinates();
        engine.canvas.setColor(RGB_COLOR8(0,128,255));
        engine.canvas.drawRect(gameState.battleField.platform);
        engine.canvas.setColor(RGB_COLOR8(0,0,0));
        engine.canvas.putPixel(gameState.battleField.platform.p1);
        engine.canvas.putPixel(gameState.battleField.platform.p2.x,
                               gameState.battleField.platform.p1.y);
        for (uint8_t r = 0; r<BLOCK_NUM_ROWS; r++)
        {
            for (uint8_t bl = 0; bl<BLOCKS_PER_ROW; bl++)
            {
                uint8_t block = gameState.battleField.blocks[r][bl];
                if (block)
                {
                     NanoRect rect = {{bl*8, r*4}, {bl*8 + 8, r*4+4}};
                     rect += blockArea.p1;
                     engine.canvas.setColor(blockColors[block]);
                     engine.canvas.fillRect(rect);
                     engine.canvas.setColor(0);
                     engine.canvas.drawRect(rect);
                }
            }
         }
         engine.canvas.setColor(RGB_COLOR8(255,255,255));
         engine.canvas.putPixel(gameState.battleField.ball);
    }
    else
    {
        char str[8] = {0};
        engine.canvas.clear();
        engine.canvas.setColor(RGB_COLOR8(255,255,255));
        engine.canvas.drawVLine(gameArea.p1.x-1,0,64);
        engine.canvas.drawVLine(gameArea.p2.x+1,0,64);
        utoa(g_score, str, 10);
        engine.canvas.setColor(RGB_COLOR8(192,192,192));
        engine.canvas.printFixed(gameArea.p2.x+3, 16, str );
    }
    return true;
}

/* Moves platform right or left according to pressed keys */
void movePlatform(void)
{
    if (engine.pressed( BUTTON_LEFT ) && (gameState.battleField.platform.p1.x > 0))
    {
        engine.refreshWorld( gameState.battleField.platform );
        gameState.battleField.platform.move(-1, 0);
        engine.refreshWorld( gameState.battleField.platform );
    }
    if (engine.pressed( BUTTON_RIGHT ) && (gameState.battleField.platform.p2.x < gameArea.width() - 1))
    {
        engine.refreshWorld( gameState.battleField.platform );
        gameState.battleField.platform.move(+1, 0);
        engine.refreshWorld( gameState.battleField.platform );
    }
}

bool checkBlockHit(void)
{
    if (!blockArea.collision(gameState.battleField.ball))
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
    gameState.battleField.blocksLeft--;
    g_score++;
    engine.refreshWorld( gameArea.p2.x + 1, 0, 95, 63 );
    if (((p.y & 3) == 2) || (p.y & 3) == 1)
    {
        gameState.battleField.ballSpeed.x = -gameState.battleField.ballSpeed.x;
    }
    else
    {
        gameState.battleField.ballSpeed.y = -gameState.battleField.ballSpeed.y;
    }
    engine.refreshWorld( gameState.battleField.ball );
    return true;
}

bool checkPlatformHit()
{
    if (gameState.battleField.platform.collisionX( gameState.battleField.ball.x ) &&
        !gameState.battleField.platform.above( gameState.battleField.ball ) &&
        !gameState.battleField.platform.below( gameState.battleField.ball ) )
    {
        if (gameState.battleField.ball.x < gameState.battleField.platform.p1.x + 3)
        {
            gameState.battleField.ballSpeed.x = -3;
            gameState.battleField.ballSpeed.y = -gameState.battleField.ballSpeed.y;
        }
        else if (gameState.battleField.ball.x > gameState.battleField.platform.p2.x - 3)
        {
            gameState.battleField.ballSpeed.x = +3;
            gameState.battleField.ballSpeed.y = -gameState.battleField.ballSpeed.y;
        }
        else
        {
            gameState.battleField.ballSpeed.x = gameState.battleField.ballSpeed.x > 0 ? 2: -2;
            gameState.battleField.ballSpeed.y = -gameState.battleField.ballSpeed.y;
        }
        return true;
    }
    return false;
}

bool checkGameAreaHit(void)
{
    bool hit = false;
    if ((gameState.battleField.ball.x < 0) ||
        (gameState.battleField.ball.x >= gameArea.width()))
    {
        hit = true;
        gameState.battleField.ballSpeed.x = -gameState.battleField.ballSpeed.x;
    }
    if ((gameState.battleField.ball.y < 0) ||
        (gameState.battleField.ball.y >= gameArea.height()))
    {
        hit = true;
        gameState.battleField.ballSpeed.y = -gameState.battleField.ballSpeed.y;
        if (gameState.battleField.ball.y >= gameArea.height())
        {
             startBattleField( false );
        }
    }
    return hit;
}

void moveBall(void)
{
    engine.refreshWorld( gameState.battleField.ball );
    bool moveBall;
    do
    {
        gameState.battleField.ballScaled += gameState.battleField.ballSpeed;
        gameState.battleField.ball = gameState.battleField.ballScaled >> PIX_BITS;
        moveBall = false;
        if (checkGameAreaHit())
        {
            moveBall = true;
        }
        if (checkPlatformHit())
        {
            moveBall = true;
        }
        if (checkBlockHit())
        {
            moveBall = true;
        }
    }
    while (moveBall);
    engine.refreshWorld( gameState.battleField.ball );
}

void battleFieldLoop(void)
{
    movePlatform();
    moveBall();
    /* Refresh debug information if we need it */
    gameState.battleField.frames++;
    if (gameState.battleField.blocksLeft == 0)
    {
        g_level++;
        startBattleField( true );
    }
}

void loadLevel(void)
{
    /* Loading level */
    if (g_level > MAX_LEVELS)
    {
       g_level = MAX_LEVELS;
    }
    gameState.battleField.blocksLeft = 0;
    for (uint8_t i =0; i<BLOCKS_PER_ROW; i++)
    {
        for (uint8_t j=0; j<BLOCK_NUM_ROWS; j++)
        {
            uint8_t block = pgm_read_byte( &levels[g_level][j][i] );
            gameState.battleField.blocks[j][i] = block;
            if (block)
            {
                gameState.battleField.blocksLeft++;
            }
        }
    }
    gameState.battleField.frames = 0;
}

void startBattleField(bool reload)
{
    engine.refresh();
    gameState.battleField.platform.setRect( 4, 56, 15, 58 );
    gameState.battleField.ball.setPoint( 9, 55);
    gameState.battleField.ballScaled = gameState.battleField.ball << PIX_BITS;
    gameState.battleField.ballSpeed.setPoint( 3, -(1<<PIX_BITS) );
    engine.drawCallback( drawBattleField );
    engine.loopCallback( battleFieldLoop );
    if ( reload )
    {
        loadLevel();
    }

    /* Show level info in popup message */
    char levelInfo[8] = "LEVEL X";
    levelInfo[6] = g_level + '0';
    engine.notify(levelInfo);
}

void setup()
{
    /* Set font to use in the game. The font has only capital letters and digits */
    ssd1306_setFixedFont(ssd1306xled_font6x8_AB);

    /* Init SPI oled display. 3 - RESET, 4 - CS (can be omitted, oled CS must be pulled down), 5 - D/C */

    /* ssd1351 must be initialized in Horizontal addressing mode */
//    ssd1351_128x128_spi_init(3, 4, 5);

    /* il9163 must be initialized in Horizontal addressing mode */
//    il9163_128x128_spi_init(3, 4, 5);

    /* ssd1331 must be initialized in Horizontal addressing mode */
    ssd1331_96x64_spi_init(3, 4, 5);

    /* Configure engine to use ZKeypand on A0 as control board. */
    engine.connectZKeypad(0);

    /* Start engine */
    engine.begin();

    /* Set frame rate. 30 fps is too slow */
    engine.setFrameRate(45);

    engine.moveTo({ -gameArea.p1.x, -gameArea.p1.y });

    startIntro();
}

void loop()
{
    if (!engine.nextFrame()) return;
    engine.display();
}
