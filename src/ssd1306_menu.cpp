/*
    MIT License

    Copyright (c) 2017-2019, Alexey Dynda

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

#include "font6x8.h"
#include "ssd1306.h"

#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef max
#define max(x,y) ((x)>(y)?(x):(y))
#endif

template <uint8_t BPP>
static uint8_t getMaxScreenItems(NanoDisplayOps<BPP> &display)
{
    return (display.height() >> 3) - 2;
}

void ssd1306_createMenu(SAppMenu *menu, const char **items, uint8_t count)
{
    menu->items = items;
    menu->count = count;
    menu->selection = 0;
    menu->oldSelection = 0;
    menu->scrollPosition = 0;
}

template <uint8_t BPP>
static uint8_t calculateScrollPosition(NanoDisplayOps<BPP> &display, SAppMenu *menu, uint8_t selection)
{
    if ( selection < menu->scrollPosition )
    {
        return selection;
    }
    else if ( selection - menu->scrollPosition > getMaxScreenItems<BPP>(display) - 1)
    {
        return selection - getMaxScreenItems<BPP>(display) + 1;
    }
    return menu->scrollPosition;
}

template <uint8_t BPP>
static void drawMenuItem(NanoDisplayOps<BPP> &display, SAppMenu *menu, uint8_t index)
{
    if (index == menu->selection)
    {
        ssd1306_negativeMode();
    }
    else
    {
        ssd1306_positiveMode();
    }
    display.setColor( 0xFF );
    display.printFixed(8, (index - menu->scrollPosition + 1)*8, menu->items[index], STYLE_NORMAL );
    ssd1306_positiveMode();
}

template <uint8_t BPP>
void ssd1306_showMenu(NanoDisplayOps<BPP> &display, SAppMenu *menu)
{
    display.drawRect(4, 4, display.width() - 5, display.height() - 5);
    menu->scrollPosition = calculateScrollPosition<BPP>( display, menu, menu->selection );
    for (uint8_t i = menu->scrollPosition; i < min(menu->count, menu->scrollPosition + getMaxScreenItems<BPP>( display )); i++)
    {
        drawMenuItem<BPP>(display, menu, i);
    }
    menu->oldSelection = menu->selection;
}

template <uint8_t BPP>
void ssd1306_updateMenu(NanoDisplayOps<BPP> &display, SAppMenu *menu)
{
    if (menu->selection != menu->oldSelection)
    {
        uint8_t scrollPosition = calculateScrollPosition<BPP>( display, menu, menu->selection );
        if ( scrollPosition != menu->scrollPosition )
        {
            display.clear();
            ssd1306_showMenu<BPP>(display, menu);
        }
        else
        {
            drawMenuItem<BPP>(display, menu, menu->oldSelection);
            drawMenuItem<BPP>(display, menu, menu->selection);
            menu->oldSelection = menu->selection;
        }
    }
}

uint8_t ssd1306_menuSelection(SAppMenu *menu)
{
    return menu->selection;
}

void ssd1306_menuDown(SAppMenu *menu)
{
    if (menu->selection < menu->count - 1)
    {
        menu->selection++;
    }
    else
    {
        menu->selection = 0;
    }
}

void ssd1306_menuUp(SAppMenu *menu)
{
    if (menu->selection > 0)
    {
        menu->selection--;
    }
    else
    {
        menu->selection = menu->count - 1;
    }
}

template
void ssd1306_showMenu<1>( NanoDisplayOps<1> &display, SAppMenu *menu);

template
void ssd1306_updateMenu<1>( NanoDisplayOps<1> &display, SAppMenu *menu);
