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
 * @file core.h Small graphics engine, based on SSD1331 functions
 */


#ifndef _NANO_ENGINE_CORE_H_
#define _NANO_ENGINE_CORE_H_

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

#endif

