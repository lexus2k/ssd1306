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

#include "nano_engine/sprite.h"
#include "nano_engine/canvas.h"
#include "nano_engine/adafruit.h"
#include "nano_engine/tiler.h"
#include "nano_engine/core.h"

// DO NOT DECLARE NanoEngine8, NanoEngine16, NanoEngine1 as class NAME: public NanoEngine<T>
// This causes flash and RAM memory consumption in compiled ELF

/**
 * @defgroup NANO_ENGINE_API NANO_ENGINE: Nano Engine description
 * @{
 * @brief Nano Engine description
 *
 * @details This group contains API functions for developing Graphics engines.
 */

/**
 * NanoEngine1 is simple graphics engine, that implements double buffering work
 * for the systems with very low resources. That is, memory buffer for SSD1306 oled
 * display needs at least 128x64/8 bytes (1024 bytes), and this is inacceptable for
 * microcontrollers like attiny85 (it has only 512B of RAM). So, to workaround
 * issue with low resources, NanoEngine1 uses small tile buffer (NE_TILE_SIZE x NE_TILE_SIZE)
 * and updates only part of oled screen at once. It makes system slow, but it is
 * possible to run NanoEngine1 on simple controllers.
 * If tile size is 32x32, then 128x64 oled display is devided into 8 tiles: <br>
 *    [0,0] [1,0] [2,0], [3,0] <br>
 *    [0,1] [1,1] [2,1], [3,1] <br>
 * In your application you can choose, if you want to refresh whole screen (refresh()), or you
 * need to refresh only part of oled display.
 *
 * @warning Works only in SSD1306 compatible mode
 */
#define NanoEngine1   NanoEngine<TILE_16x16_MONO>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define NanoEngineArduboy   NanoEngine<TILE_8x8_MONO>
#endif

/**
 * NanoEngine8 is simple graphics engine, that implements double buffering work
 * for the systems with very low resources. That is, memory buffer for SSD1331 oled
 * display needs at least 96x64x1 bytes (6144 bytes), and this is inacceptable for
 * microcontrollers like atmega328p (it has only 2KiB of RAM). So, to workaround
 * issue with low resources, NanoEngine8 uses small tile buffer (NE_TILE_SIZE x NE_TILE_SIZE)
 * and updates only part of oled screen at once. It makes system slow, but it is
 * possible to run NanoEngine8 on simple controllers.
 * If tile size is 32x32, then 96x64 oled display is devided into 6 tiles: <br>
 *    [0,0] [1,0] [2,0] <br>
 *    [0,1] [1,1] [2,1] <br>
 * In your application you can choose, if you want to refresh whole screen (refresh()), or you
 * need to refresh only part of oled display.
 */
#define NanoEngine8   NanoEngine<TILE_16x16_RGB8>

/**
 * NanoEngine1 is simple graphics engine, that implements double buffering work
 * for the systems with very low resources. That is, memory buffer for SSD1306 oled
 * display needs at least 128x64/8 bytes (1024 bytes), and this is inacceptable for
 * microcontrollers like attiny85 (it has only 512B of RAM). So, to workaround
 * issue with low resources, NanoEngine1 uses small tile buffer (NE_TILE_SIZE x NE_TILE_SIZE)
 * and updates only part of oled screen at once. It makes system slow, but it is
 * possible to run NanoEngine1 on simple controllers.
 * If tile size is 32x32, then 128x64 oled display is devided into 8 tiles: <br>
 *    [0,0] [1,0] [2,0], [3,0] <br>
 *    [0,1] [1,1] [2,1], [3,1] <br>
 * In your application you can choose, if you want to refresh whole screen (refresh()), or you
 * need to refresh only part of oled display.
 *
 * @warning Works only in RGB 8-bit color mode
 */
class NanoEngine1_8: public NanoEngine<TILE_8x8_MONO_8>
{
public:
    /**
     * Creates new Graphics Engine object.
     */
    NanoEngine1_8(): NanoEngine() {};
};

/**
 * NanoEngine16 is simple graphics engine, that implements double buffering work
 * for the systems with very low resources. That is, memory buffer for SSD1351 oled
 * display needs at least 128x128x2 bytes (32768 bytes), and this is inacceptable for
 * microcontrollers like atmega328p (it has only 2KiB of RAM). So, to workaround
 * issue with low resources, NanoEngine16 uses small tile buffer (NE_TILE_SIZE x NE_TILE_SIZE)
 * and updates only part of oled screen at once. It makes system slow, but it is
 * possible to run NanoEngine16 on simple controllers.
 * If tile size is 16x16, then 128x128 oled display is devided into 64 tiles: <br>
 *    [0,0] [1,0] [2,0] [3,0] [4,0] [5,0] [6,0] [7,0] <br>
 *    [0,1] [1,1] [2,1] [3,1] [4,1] [5,1] [6,1] [7,1] <br>
 *    etc. <br>
 * In your application you can choose, if you want to refresh whole screen (refresh()), or you
 * need to refresh only part of oled display.
 */
#define NanoEngine16  NanoEngine<TILE_8x8_RGB16>

/**
 * @}
 */

#endif

