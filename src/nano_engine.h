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

/**
 * This class is responsible for holding and updating data about areas to be refreshed
 * on LCD display.
 */
class NanoEngineTiler
{
protected:
    /* Only child classes can initialize the engine */
    NanoEngineTiler();

public:
    /** Number of bits in tile size. 5 corresponds to 1<<5 = 32 tile size */
    static const uint8_t NE_TILE_SIZE_BITS = 3;
    /** Max tiles supported in X */
    static const uint8_t NE_MAX_TILES_NUM = 16 >> (NE_TILE_SIZE_BITS - 3);

    /**
     * Marks all tiles for update. Actual update will take place in display() method.
     */
    static void refresh();

    /**
     * Marks specific tile for update. Actual update will take place in display() method.
     * @param tileX - x index of tile to update
     * @param tileY - y index of tile to update
     */
    static void refreshTile(uint8_t tileX, uint8_t tileY) { m_refreshFlags[tileY] |= (1<<tileX); };

    /**
     * Mark specified area in pixels for redrawing by NanoEngine.
     * Actual update will take place in display() method.
     */
    static void refresh(const NanoRect &rect);

    /**
     * Mark specified area in pixels for redrawing by NanoEngine.
     * Actual update will take place in display() method.
     */
    static void refresh(const NanoPoint &point);

    /**
     * Mark specified area in pixels for redrawing by NanoEngine.
     * Actual update will take place in display() method.
     */
    static void refresh(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

protected:
    /**
     * Contains information on tiles to be updated.
     * Elements of array are rows and bits are columns.
     */
    static uint16_t   m_refreshFlags[NE_MAX_TILES_NUM];
};

/**
 * Base class for NanoEngine.
 */
class NanoEngineBase
{
public:
    /**
     * Initializes Nano Engine Base object.
     */
    NanoEngineBase();

    /**
     * Initializes internal timestamps.
     */
    static void begin();

    /**
     * Sets working frame-rate for the engine
     * @param fps - frame rate to set between [1-255]
     */
    static void setFrameRate(uint8_t fps);
 
    /**
     * Returns current frame rate
     */
    static uint8_t getFrameRate() { return m_fps; };

    /**
     * Returns cpu load in percents [0-255].
     * 100 means maximum normal CPU load.
     * 0 means, CPU has nothing to do.
     * >100 means that CPU is not enough to perform all operations
     */
    static uint8_t getCpuLoad() { return m_cpuLoad; };

    /**
     * Returns true if it is time to render next frame
     */
    static bool nextFrame();

    /**
     * Sets user-defined draw callback. This callback will be called everytime, engine needs
     * to update display content. If callback returns false, engine will not update those area.
     * You always have a way to find out, which area is being updated by engine via
     * NanoEngine8::canvas::getOffset() and NE_TILE_SIZE.
     * @param callback - user-defined draw callback.
     * @note you can change draw callback anytime you need.
     */
    static void drawCallback(TNanoEngineOnDraw callback) { m_onDraw = callback; };

    /**
     * Sets user-defined loop callback. This callback will be called once every time
     * new frame needs to be refreshed on oled display.
     */
    static void loopCallback(TLoopCallback callback) { m_loop = callback; };

    /**
     * @brief Returns true if button or specific combination of buttons is not pressed.
     * Returns true if button or specific combination of buttons is pressed.
     * @param buttons - buttons to check
     * @return true or false
     */
    static bool pressed(uint8_t buttons);

    /**
     * @brief Returns true if button or specific combination of buttons is not pressed.
     * Returns true if button or specific combination of buttons is not pressed.
     * @param buttons - buttons to check
     * @return true of false
     */
    static bool notPressed(uint8_t buttons);

    /**
     * @brief Returns bits of all pressed buttons
     * Returns bits of all pressed buttons
     */
    static uint8_t buttonsState()
    {
        return m_onButtons();
    }

    /**
     * Configures NanoEngine8 to use custom key handler.
     * You can implement in your handler any keyboard layout, you use in your schematics.
     */
    static void connectCustomKeys(TNanoEngineGetButtons handler);

    /**
     * @brief Enables engine to use Z-Keypad.
     * Enables engine to use Z-Keypad. Please refer to arkanoid example for schematics.
     * @param analogPin - pin, which Z-Keypad is connected to.
     */
    static void connectZKeypad(uint8_t analogPin);

    /**
     * @brief Configures NanoEngine8 to use Arduboy keys layout.
     * Configures NanoEngine8 to use Arduboy keys layout.
     */
    static void connectArduboyKeys();

protected:
    /** Duration between frames in milliseconds */
    static uint8_t   m_frameDurationMs;
    /** Current fps */
    static uint8_t   m_fps;
    /** Current cpu load in percents */
    static uint8_t   m_cpuLoad;
    /** Last timestamp in milliseconds the frame was updated on oled display */
    static uint32_t  m_lastFrameTs;
    /** Callback to call if specific tile needs to be updated */
    static TNanoEngineOnDraw m_onDraw;
    /** Callback to call if buttons state needs to be updated */
    static TNanoEngineGetButtons m_onButtons;
    /** Callback to call before starting oled update */
    static TLoopCallback m_loop;

private:
    static uint8_t s_zkeypadPin;
    static uint8_t zkeypadButtons();
    static uint8_t arduboyButtons();
};

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
 * In your application you can choose, if you want to refresh whole screen (refresh()), or you
 * need to refresh only part of oled display (refreshTile()).
 */
class NanoEngine8: public NanoEngineBase, public NanoEngineTiler
{
public:
    /** object, representing canvas. Use it in your draw handler */
    static NanoCanvas8 canvas;

    /**
     * Creates new Graphics Engine object.
     */
    NanoEngine8();

    /**
     * Initializes engine, sets required mode for OLED display
     */
    static void begin();

    /**
     * @brief refreshes content on oled display.
     * Refreshes content on oled display. Call it, if you want to update the screen.
     * Engine will update only those areas, which are marked by refreshAll()/refreshTile()
     * methods.
     */
    static void display();

private:
    static uint8_t   m_buffer[(1<<NE_TILE_SIZE_BITS) * (1<<NE_TILE_SIZE_BITS)];
};

/**
 * NanoEngine1 is simple graphics engine, that implements double buffering work
 * for the systems with very low resources. That is, memory buffer for SSD1306 oled
 * display needs at least 128x64/8 bytes (1024 bytes), and this is inacceptable for
 * microcontrollers like attiny85 (it has only 512B of RAM). So, to workaround
 * issue with low resources, NanoEngine1 uses small tile buffer (NE_TILE_SIZE x NE_TILE_SIZE)
 * and updates only part of oled screen at once. It makes system slow, but it is
 * possible to run NanoEngine1 on simple controllers.
 * If tile size is 32x32, then 128x64 oled display is devided into 8 tiles:
 *    [0,0] [1,0] [2,0], [3,0]
 *    [0,1] [1,1] [2,1], [3,1]
 * In your application you can choose, if you want to refresh whole screen (refresh()), or you
 * need to refresh only part of oled display (refreshTile()).
 */
class NanoEngine1: public NanoEngineBase, public NanoEngineTiler
{
public:
    /** object, representing canvas. Use it in your draw handler */
    static NanoCanvas1 canvas;

    /**
     * Creates new Graphics Engine object.
     */
    NanoEngine1();

    /**
     * Initializes engine, sets required mode for OLED display
     */
    static void begin();

    /**
     * @brief refreshes content on oled display.
     * Refreshes content on oled display. Call it, if you want to update the screen.
     * Engine will update only those areas, which are marked by refreshAll()/refreshTile()
     * methods.
     */
    static void display();

private:
    static uint8_t   m_buffer[(1<<NE_TILE_SIZE_BITS) * (1<<NE_TILE_SIZE_BITS) / 8];
};

#endif

