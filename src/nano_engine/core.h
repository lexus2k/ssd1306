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

#include "tiler.h"
#include "nano_canvas.h"

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
 * Class for keys processing functionality
 */
class NanoEngineInputs
{
protected:
    /**
     * Initializes Nano Engine Inputs object.
     */
    NanoEngineInputs() {};
public:
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
    /** Callback to call if buttons state needs to be updated */
    static TNanoEngineGetButtons m_onButtons;

private:
    static uint8_t s_zkeypadPin;
    static uint8_t zkeypadButtons();
    static uint8_t arduboyButtons();
};

/**
 * Base class for NanoEngine.
 */
template<class C, uint8_t W, uint8_t H, uint8_t B>
class NanoEngineCore: public NanoEngineInputs, public NanoEngineTiler<C,W,H,B>
{
public:
    /**
     * Initializes Nano Engine Base object.
     */
    NanoEngineCore();

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
     * Sets user-defined loop callback. This callback will be called once every time
     * new frame needs to be refreshed on oled display.
     */
    static void loopCallback(TLoopCallback callback) { m_loop = callback; };

    /**
     * @brief refreshes content on oled display.
     * Refreshes content on oled display. Call it, if you want to update the screen.
     * Engine will update only those areas, which are marked by refresh()
     * methods.
     */
    static void display();

    /**
     * @brief shows notification to a user for 1 seconds
     * Shows notification to a user for 1 seconds
     * @param str - pointer to null-terminated string to show
     */
    static void notify(const char *str);

protected:
    /** Duration between frames in milliseconds */
    static uint8_t   m_frameDurationMs;
    /** Current fps */
    static uint8_t   m_fps;
    /** Current cpu load in percents */
    static uint8_t   m_cpuLoad;
    /** Last timestamp in milliseconds the frame was updated on oled display */
    static uint32_t  m_lastFrameTs;
    /** Callback to call before starting oled update */
    static TLoopCallback m_loop;
};

/** Defaut frame rate for all engines */
static const uint8_t ENGINE_DEFAULT_FPS = 30;

/** Duration between frames in milliseconds */
template<class C, uint8_t W, uint8_t H, uint8_t B>
uint8_t   NanoEngineCore<C,W,H,B>::m_frameDurationMs = 1000/ENGINE_DEFAULT_FPS;
/** Current fps */
template<class C, uint8_t W, uint8_t H, uint8_t B>
uint8_t   NanoEngineCore<C,W,H,B>::m_fps = ENGINE_DEFAULT_FPS;
/** Current cpu load in percents */
template<class C, uint8_t W, uint8_t H, uint8_t B>
uint8_t   NanoEngineCore<C,W,H,B>::m_cpuLoad = 0;
/** Last timestamp in milliseconds the frame was updated on oled display */
template<class C, uint8_t W, uint8_t H, uint8_t B>
uint32_t  NanoEngineCore<C,W,H,B>::m_lastFrameTs;
/** Callback to call before starting oled update */
template<class C, uint8_t W, uint8_t H, uint8_t B>
TLoopCallback NanoEngineCore<C,W,H,B>::m_loop = nullptr;

template<class C, uint8_t W, uint8_t H, uint8_t B>
NanoEngineCore<C,W,H,B>::NanoEngineCore()
    : NanoEngineInputs()
{
}

template<class C, uint8_t W, uint8_t H, uint8_t B>
void NanoEngineCore<C,W,H,B>::begin()
{
    m_lastFrameTs = millis();
}

template<class C, uint8_t W, uint8_t H, uint8_t B>
void NanoEngineCore<C,W,H,B>::setFrameRate(uint8_t fps)
{
    m_fps = fps;
    m_frameDurationMs = 1000/fps;
}

template<class C, uint8_t W, uint8_t H, uint8_t B>
bool NanoEngineCore<C,W,H,B>::nextFrame()
{
    bool needUpdate = (uint32_t)(millis() - m_lastFrameTs) >= m_frameDurationMs;
    if (needUpdate && m_loop) m_loop();
    return needUpdate;
}

template<class C, uint8_t W, uint8_t H, uint8_t B>
void NanoEngineCore<C,W,H,B>::display()
{
    m_lastFrameTs = millis();
    NanoEngineTiler<C,W,H,B>::displayBuffer();
    m_cpuLoad = ((millis() - m_lastFrameTs)*100)/m_frameDurationMs;
}

template<class C, uint8_t W, uint8_t H, uint8_t B>
void NanoEngineCore<C,W,H,B>::notify(const char *str)
{
    NanoEngineTiler<C,W,H,B>::displayPopup(str);
    delay(1000);
    m_lastFrameTs = millis();
    NanoEngineTiler<C,W,H,B>::refresh();
}


#endif

