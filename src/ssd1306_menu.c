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

extern SFixedFontInfo s_fixedFont;
extern uint16_t ssd1306_color;

static uint8_t getMaxScreenItems(void)
{
    return ((ssd1306_displayHeight() - 16) / (s_fixedFont.pages * 8));
}

static uint8_t getMaxScreenItems8(void)
{
    return ((ssd1306_displayHeight() - 16) / (s_fixedFont.h.height + 0));
}

void ssd1306_createMenu(SAppMenu *menu, const char **items, uint8_t count)
{
    menu->items = items;
    menu->count = count;
    menu->selection = 0;
    menu->oldSelection = 0;
    menu->scrollPosition = 0;
}

static uint8_t calculateScrollPosition(SAppMenu *menu, uint8_t selection)
{
    if ( selection < menu->scrollPosition )
    {
        return selection;
    }
    else if ( selection - menu->scrollPosition > getMaxScreenItems() - 1)
    {
        return selection - getMaxScreenItems() + 1;
    }
    return menu->scrollPosition;
}

static uint8_t calculateScrollPosition8(SAppMenu *menu, uint8_t selection)
{
    if ( selection < menu->scrollPosition )
    {
        return selection;
    }
    else if ( selection - menu->scrollPosition > getMaxScreenItems8() - 1)
    {
        return selection - getMaxScreenItems8() + 1;
    }
    return menu->scrollPosition;
}

static void drawMenuItem(SAppMenu *menu, uint8_t index)
{
    if (index == menu->selection)
    {
        ssd1306_negativeMode();
    }
    else
    {
        ssd1306_positiveMode();
    }
    ssd1306_printFixed(8, (index - menu->scrollPosition)* (s_fixedFont.pages * 8) + 8, menu->items[index], STYLE_NORMAL );
    ssd1306_positiveMode();
}

static void drawMenuItem8(SAppMenu *menu, uint8_t index)
{
    if (index == menu->selection)
    {
        ssd1306_negativeMode();
    }
    else
    {
        ssd1306_positiveMode();
    }
    ssd1306_printFixed8(8, (index - menu->scrollPosition)*(s_fixedFont.h.height + 0) + 8, menu->items[index], STYLE_NORMAL );
    ssd1306_positiveMode();
}

static void drawMenuItem16(SAppMenu *menu, uint8_t index)
{
    if (index == menu->selection)
    {
        ssd1306_negativeMode();
    }
    else
    {
        ssd1306_positiveMode();
    }
    ssd1306_printFixed16(8, (index - menu->scrollPosition) * (s_fixedFont.h.height + 0) + 8, menu->items[index], STYLE_NORMAL );
    ssd1306_positiveMode();
}

void ssd1306_showMenu(SAppMenu *menu)
{
    ssd1306_drawRect(4, 4, ssd1306_displayWidth() - 5, ssd1306_displayHeight() - 5);
    menu->scrollPosition = calculateScrollPosition( menu, menu->selection );
    for (uint8_t i = menu->scrollPosition; i < min(menu->count, menu->scrollPosition + getMaxScreenItems()); i++)
    {
        drawMenuItem(menu, i);
    }
    menu->oldSelection = menu->selection;
}

void ssd1306_showMenu8(SAppMenu *menu)
{
    ssd1306_drawRect8(4, 4, ssd1306_displayWidth() - 5, ssd1306_displayHeight() - 5);
    menu->scrollPosition = calculateScrollPosition8( menu, menu->selection );
    for (uint8_t i = menu->scrollPosition; i < min(menu->count, menu->scrollPosition + getMaxScreenItems8()); i++)
    {
        drawMenuItem8(menu, i);
    }
    menu->oldSelection = menu->selection;
}

void ssd1306_showMenu16(SAppMenu *menu)
{
    ssd1306_drawRect16(4, 4, ssd1306_displayWidth() - 5, ssd1306_displayHeight() - 5);
    menu->scrollPosition = calculateScrollPosition8( menu, menu->selection );
    for (uint8_t i = menu->scrollPosition; i < min(menu->count, menu->scrollPosition + getMaxScreenItems8()); i++)
    {
        drawMenuItem16(menu, i);
    }
    menu->oldSelection = menu->selection;
}

void ssd1306_updateMenu(SAppMenu *menu)
{
    if (menu->selection != menu->oldSelection)
    {
        uint8_t scrollPosition = calculateScrollPosition( menu, menu->selection );
        if ( scrollPosition != menu->scrollPosition )
        {
            ssd1306_clearScreen();
            ssd1306_showMenu(menu);
        }
        else
        {
            drawMenuItem(menu, menu->oldSelection);
            drawMenuItem(menu, menu->selection);
            menu->oldSelection = menu->selection;
        }
    }
}

void ssd1306_updateMenu8(SAppMenu *menu)
{
    if (menu->selection != menu->oldSelection)
    {
        uint8_t scrollPosition = calculateScrollPosition8( menu, menu->selection );
        if ( scrollPosition != menu->scrollPosition )
        {
            ssd1306_clearScreen8();
            ssd1306_showMenu8(menu);
        }
        else
        {
            drawMenuItem8(menu, menu->oldSelection);
            drawMenuItem8(menu, menu->selection);
            menu->oldSelection = menu->selection;
        }
    }
}

void ssd1306_updateMenu16(SAppMenu *menu)
{
    if (menu->selection != menu->oldSelection)
    {
        uint8_t scrollPosition = calculateScrollPosition8( menu, menu->selection );
        if ( scrollPosition != menu->scrollPosition )
        {
            ssd1306_clearScreen16();
            ssd1306_showMenu16(menu);
        }
        else
        {
            drawMenuItem16(menu, menu->oldSelection);
            drawMenuItem16(menu, menu->selection);
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

void ssd1306_drawProgressBar(int8_t progress)
{
    lcduint_t height = 8;
    lcduint_t width = 8;
    char str[5] = "100%";
    if ( progress < 100 )
    {
        str[0] = ' ';
        str[1] = progress / 10 + '0';
        str[2] = progress % 10 + '0';
        str[3] = '%';
    }
    if ( s_fixedFont.primary_table != NULL )
    {
        width = ssd1306_getTextSize( str, &height );
    }
    lcdint_t middle = ssd1306_displayHeight() / 2;
    lcdint_t progress_pos = 8 + (int16_t)(ssd1306_displayWidth() - 16) * progress / 100;
    uint16_t color = ssd1306_color;
    ssd1306_color = 0x0000;
    ssd1306_fillRect( progress_pos, middle, ssd1306_displayWidth() - 8, middle + height );
    ssd1306_color = color;
    ssd1306_printFixed( ssd1306_displayWidth() / 2 - width / 2, middle - height, str, STYLE_NORMAL );
    ssd1306_drawRect( 8, middle, ssd1306_displayWidth() - 8, middle + height );
    ssd1306_fillRect( 8, middle, progress_pos, middle + height );
}

void ssd1306_drawProgressBar8(int8_t progress)
{
    lcduint_t height = 8;
    lcduint_t width = 8;
    char str[5] = "100%";
    if ( progress < 100 )
    {
        str[0] = ' ';
        str[1] = progress / 10 + '0';
        str[2] = progress % 10 + '0';
        str[3] = '%';
    }
    if ( s_fixedFont.primary_table != NULL )
    {
        width = ssd1306_getTextSize( str, &height );
    }
    lcdint_t middle = ssd1306_displayHeight() / 2;
    lcdint_t progress_pos = 8 + (int16_t)(ssd1306_displayWidth() - 16) * progress / 100;
    uint16_t color = ssd1306_color;
    ssd1306_color = 0x0000;
    ssd1306_fillRect8( progress_pos, middle, ssd1306_displayWidth() - 8, middle + height );
    ssd1306_color = color;
    ssd1306_printFixed8( ssd1306_displayWidth() / 2 - width / 2, middle - height, str, STYLE_NORMAL );
    ssd1306_drawRect8( 8, middle, ssd1306_displayWidth() - 8, middle + height );
    ssd1306_fillRect8( 8, middle, progress_pos, middle + height );
}
