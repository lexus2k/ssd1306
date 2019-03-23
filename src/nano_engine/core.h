/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

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
#include "canvas/canvas.h"

/**
 * @ingroup NANO_ENGINE_API
 * @{
 */

/** Defaut frame rate for all engines */
#define ENGINE_DEFAULT_FPS  (30)

/** Type of user-specified keyboard callback */
typedef uint8_t (*TNanoEngineGetButtons)(void);

/** Type of user-specified loop callback */
typedef void (*TLoopCallback)(void);

///////////////////////////////////////////////////////////////////////////////
////// NANO ENGINE INPUTS CLASS ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

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
     *
     * Returns bits of all pressed buttons. For example, to check if Down button is pressed
     * you need to write `if (result & BUTTON_DOWN) {}`. Available constants are:
     * BUTTON_DOWN, BUTTON_LEFT, BUTTON_RIGHT, BUTTON_UP, BUTTON_A, BUTTON_B.
     *
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    /**
     * @brief Configures NanoEngine to use KY40 Rotary Encoder.
     * Configures NanoEngine to use KY40 Rotary Encoder.
     * @param pina_clk pin number to use as clk (see KY40 docs).
     * @param pinb_dt pin number to use as direction pin (see KY40 docs).
     * @param pinc_sw optional pin number ot use as push button.
     * @warning do not use, not tested
     */
    static void connectKY40encoder(uint8_t pina_clk, uint8_t pinb_dt, int8_t pinc_sw = -1);
#endif

    /**
     * @brief Enables engine to use GPIO keys
     *
     * Enables engine  to use gpio-keys. You need to pass globally defined array,
     * containing GPIO pin numbers for the 6 buttons in the following order:
     * Down, Left, Right, Up, A, B. If you don't want to use some specific button,
     * then just set not used button to 0.
     * Once you call this function, you can read buttons state via buttonsState().
     *
     * @param gpioKeys pointer to 6-button pins array.
     *
     * @see buttonsState()
     */
    static void connectGpioKeypad(const uint8_t *gpioKeys);

protected:
    /** Callback to call if buttons state needs to be updated */
    static TNanoEngineGetButtons m_onButtons;

private:
    static uint8_t s_zkeypadPin;
    static const uint8_t * s_gpioKeypadPins;
    static uint8_t s_ky40_clk;
    static uint8_t s_ky40_dt;
    static uint8_t s_ky40_sw;
    static uint8_t zkeypadButtons();
    static uint8_t arduboyButtons();
    static uint8_t gpioButtons();
    static uint8_t ky40Buttons();
};


///////////////////////////////////////////////////////////////////////////////
////// NANO ENGINE CORE CLASS /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 * Nano Engine Core class, contains generic frame-rate control functions
 */
class NanoEngineCore: public NanoEngineInputs
{
protected:
    NanoEngineCore(): NanoEngineInputs() {};

public:
    /**
     * Initializes internal timestamps.
     */
    void begin();

    /**
     * Sets working frame-rate for the engine
     * @param fps - frame rate to set between [1-255]
     */
    void setFrameRate(uint8_t fps);
 
    /**
     * Returns current frame rate
     */
    uint8_t getFrameRate() { return m_fps; };

    /**
     * Returns cpu load in percents [0-255].
     * 100 means maximum normal CPU load.
     * 0 means, CPU has nothing to do.
     * >100 means that CPU is not enough to perform all operations
     */
    uint8_t getCpuLoad() { return m_cpuLoad; };

    /**
     * Returns true if it is time to render next frame
     */
    bool nextFrame();

    /**
     * Sets user-defined loop callback. This callback will be called once every time
     * new frame needs to be refreshed on oled display.
     */
    void loopCallback(TLoopCallback callback) { m_loop = callback; };

protected:

    /** Duration between frames in milliseconds */
    uint8_t   m_frameDurationMs = 1000/ENGINE_DEFAULT_FPS;
    /** Current fps */
    uint8_t   m_fps = ENGINE_DEFAULT_FPS;
    /** Current cpu load in percents */
    uint8_t   m_cpuLoad = 0;
    /** Last timestamp in milliseconds the frame was updated on oled display */
    uint32_t  m_lastFrameTs;
    /** Callback to call before starting oled update */
    TLoopCallback m_loop = nullptr;
};

/**
 * Base class for NanoEngine.
 */
template<class C, class D>
class NanoEngine: public NanoEngineCore,
                  public NanoEngineTiler<C,D>
{
public:
    /**
     * Initializes Nano Engine Base object.
     */
    NanoEngine(D &display);

    /**
     * @brief refreshes content on oled display.
     * Refreshes content on oled display. Call it, if you want to update the screen.
     * Engine will update only those areas, which are marked by refresh()
     * methods.
     */
    void display();

    /**
     * Initializes internal timestamps, engine state, and
     * switches oled display to required mode (see ssd1306_setMode()).
     */
    void begin();

    /**
     * @brief shows notification to a user for 1 seconds
     * Shows notification to a user for 1 seconds
     * @param str - pointer to null-terminated string to show
     */
    void notify(const char *str);

protected:
};

template<class C, class D>
NanoEngine<C,D>::NanoEngine( D & display)
    : NanoEngineCore(), NanoEngineTiler<C,D>(display)
{
}

template<class C, class D>
void NanoEngine<C,D>::display()
{
    m_lastFrameTs = millis();
    NanoEngineTiler<C,D>::displayBuffer();
    m_cpuLoad = ((millis() - m_lastFrameTs)*100)/m_frameDurationMs;
}

template<class C, class D>
void NanoEngine<C,D>::begin()
{
    NanoEngineCore::begin();
}

template<class C, class D>
void NanoEngine<C,D>::notify(const char *str)
{
    NanoEngineTiler<C,D>::displayPopup(str);
    delay(1000);
    m_lastFrameTs = millis();
    NanoEngineTiler<C,D>::refresh();
}

/**
 * @}
 */

#endif

