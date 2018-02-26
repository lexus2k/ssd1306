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
 * @file nano_engine.h Small graphics engine, based on SSD1331 functions
 */


#ifndef _NANO_ENGINE_H_
#define _NANO_ENGINE_H_

#include "nano_canvas.h"

/** Type of user-specified draw callback */
typedef bool (*TNanoEngineOnDraw)(void);

/** Type of user-specified keyboard callback */
typedef uint8_t (*TNanoEngineGetButtons)(void);

/** Type of user-specified loop callback */
typedef void (*TLoopCallback)(void);

enum
{
    BUTTON_NONE   = 0B00000000,
    BUTTON_DOWN   = 0B00000001,
    BUTTON_LEFT   = 0B00000010,
    BUTTON_RIGHT  = 0B00000100,
    BUTTON_UP     = 0B00001000,
    BUTTON_A      = 0B00010000,
    BUTTON_B      = 0B00100000,
};

typedef struct
{
    lcdint_t x;
    lcdint_t y;
} NanoPoint;

/**
 * NanoEngine8 is simple graphics engine, that implements double buffering work
 * for the systems with very low resources. That is, memory buffer for SSD1331 oled
 * display needs at least 96x64x1 bytes (6144 bytes), and this is inacceptable for
 * microcontrollers like atmega328p (it has only 2KiB of RAM). So, to workaround
 * issue with low resources, NanoEngine8 uses small tile buffer (NE_TILE_SIZE x NE_TILE_SIZE)
 * and updates only part of oled screen at once. It makes system slow, but it is
 * possible to run NanoEngine8 on simple controllers.
 * If tile size is 32x32, then 96x64 oled display is devided into 6 tiles:
 *    [0,0] [1,0] [2,0]
 *    [0,1] [1,1] [2,1]
 * In your application you can choose, if you want to refresh whole screen (refreshAll()), or you
 * need to refresh only part of oled display (refreshTile()).
 */
class NanoEngine8
{
public:
    /** Number of bits in tile size. 5 corresponds to 1<<5 = 32 tile size */
    static const uint8_t NE_TILE_SIZE_BITS = 4;
    /** Tile size in pixels */
    static const uint8_t NE_TILE_SIZE = (1<<NE_TILE_SIZE_BITS);
    /** Max tiles supported in X */
    static const uint8_t NE_MAX_TILES_NUM = 8 >> (NE_TILE_SIZE_BITS - 4);

    /** object, representing canvas. Use it in your draw handler */
    NanoCanvas8 canvas;

    /**
     * Creates new Graphics Engine object.
     */
    NanoEngine8();

    void begin();

    void setFrameRate(uint8_t fps);
 
    uint8_t getFrameRate() { return m_fps; };

    bool nextFrame();

    /**
     * @brief refreshes content on oled display.
     * Refreshes content on oled display. Call it, if you want to update the screen.
     * Engine will update only those areas, which are marked by refreshAll()/refreshTile()
     * methods.
     */
    void display();

    /**
     * Marks all tiles for update. Actual update will take place in display() method.
     */
    void refreshAll();

    /**
     * Marks specific tile for update. Actual update will take place in display() method.
     * @param tileX - x index of tile to update
     * @param tileY - y index of tile to update
     */
    void refreshTile(uint8_t tileX, uint8_t tileY) { m_refreshFlags[tileY] |= (1<<tileX); };

    void refreshRect(NanoRect &rect);

    void refreshRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

    /**
     * Sets user-defined draw callback. This callback will be called everytime, engine needs
     * to update display content. If callback returns false, engine will not update those area.
     * You always have a way to find out, which area is being updated by engine via
     * NanoEngine8::canvas::getOffset() and NE_TILE_SIZE.
     * @param callback - user-defined draw callback.
     * @note you can change draw callback anytime you need.
     */
    void drawCallback(TNanoEngineOnDraw callback) { m_onDraw = callback; };

    void loopCallback(TLoopCallback callback) { m_loop = callback; };

    /**
     * @brief Returns true if button or specific combination of buttons is not pressed.
     * Returns true if button or specific combination of buttons is pressed.
     * @param buttons - buttons to check
     * @return true or false
     */
    bool pressed(uint8_t buttons);

    /**
     * @brief Returns true if button or specific combination of buttons is not pressed.
     * Returns true if button or specific combination of buttons is not pressed.
     * @param buttons - buttons to check
     * @return true of false
     */
    bool notPressed(uint8_t buttons);

    /**
     * Configures NanoEngine8 to use custom key handler.
     * You can implement in your handler any keyboard layout, you use in your schematics.
     */
    void connectCustomKeys(TNanoEngineGetButtons handler);

    /**
     * @brief Enables engine to use Z-Keypad.
     * Enables engine to use Z-Keypad. Please refer to arkanoid example for schematics.
     * @param analogPin - pin, which Z-Keypad is connected to.
     */
    void connectZKeypad(uint8_t analogPin);

    /**
     * @brief Configures NanoEngine8 to use Arduboy keys layout.
     * Configures NanoEngine8 to use Arduboy keys layout.
     */
    void connectArduboyKeys();

private:
    uint8_t   m_buffer[NE_TILE_SIZE * NE_TILE_SIZE];
    uint8_t   m_refreshFlags[NE_MAX_TILES_NUM];
    uint8_t   m_frameDurationMs;
    uint8_t   m_fps;
    uint32_t  m_lastFrameTs;
    TNanoEngineOnDraw m_onDraw;
    TNanoEngineGetButtons m_onButtons;
    TLoopCallback m_loop;

    static uint8_t s_zkeypadPin;
    static uint8_t zkeypadButtons();
    static uint8_t arduboyButtons();
};

#endif

