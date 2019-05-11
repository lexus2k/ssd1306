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

///////////////////////////////////////////////////////////////////////
// This sketch will run on Arduboy HW (compile for Leonardo board)
//////////////////////////////////////////////////////////////////////

#include "ssd1306.h"
#include "nano_engine.h"

/* Do not use NanoEngine1, because there is issue in Arduboy emulator */
NanoEngineArduboy engine;

/* Define rect to move on the display */
NanoRect rect = { {15,12}, {60,35} }; 

/*
 * This function is called every time engine needs to refresh display content.
 * Just draw here all you need as usual.
 * If the function returns false, then block, pointed by engine.canvas.offset will
 * not be refreshed
 */
bool drawAll()
{
    engine.canvas.clear();
    engine.canvas.setColor(RGB_COLOR8(255,255,0));
    engine.canvas.drawRect(rect);   // draw rect in buffer
    return true;
}

void setup()
{
    /* Init Mono OLED 128x64 for Arduboy. 6 - RESET, 12 - CS, 4 - D/C */
    ssd1306_128x64_spi_init(6, 12, 4);
    /* initialize engine */
    engine.begin();
    engine.setFrameRate(45);          // Set frame rate
    engine.drawCallback( drawAll );  // Set callback to draw content
    engine.connectArduboyKeys();     // Connect Arduboy keypad
    engine.refresh();                // Makes engine to refresh whole display content at start-up
}

void loop()
{
    if (!engine.nextFrame()) return; // exit if we're still waiting for next frame
    NanoPoint point = {0,0};  // calculate vector to move rect
    if (engine.pressed( BUTTON_RIGHT )) point.x = +1;
    if (engine.pressed( BUTTON_LEFT ))  point.x = -1;
    if (engine.pressed( BUTTON_UP ))    point.y = -1;
    if (engine.pressed( BUTTON_DOWN ))  point.y = +1;
    engine.refresh(rect);            // Update screen content at old rect position
    rect += point;                   // Move rect according to pressed keys
    engine.refresh(rect);            // Update screen content at new rect position
    engine.display();                // refresh display content
}

