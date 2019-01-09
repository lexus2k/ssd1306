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
 * @file tiler.h Tiler helper for graphics processing
 */


#ifndef _NANO_ENGINE_TILER_H_
#define _NANO_ENGINE_TILER_H_

#include "canvas.h"
#include "lcd/lcd_common.h"

/**
 * @ingroup NANO_ENGINE_API
 * @{
 */

/**
 * Structure, holding currently set font.
 * @warning Only for internal use.
 */
extern "C" SFixedFontInfo s_fixedFont;

/* The table below defines arguments for NanoEngineTiler.          *
 *                            canvas        width   height  bits   */
// Tiles for monochrome displays
#define TILE_128x64_MONO      NanoCanvas1,  128,    64,     7    ///< Full-screen 1-bit tile for SSD1306
#define TILE_8x8_MONO         NanoCanvas1,  8,      8,      3    ///< Standard 1-bit tile 8x8 for monochrome mode
#define TILE_16x16_MONO       NanoCanvas1,  16,     16,     4    ///< Standard 1-bit tile 16x16 for monochrome mode
#define TILE_32x32_MONO       NanoCanvas1,  32,     32,     4    ///< Standard 1-bit tile 32x32 for monochrome mode
// Tiles for 8-bit displays
#define TILE_8x8_RGB8         NanoCanvas8,  8,      8,      3    ///< Standard 8-bit RGB tile 8x8
#define TILE_16x16_RGB8       NanoCanvas8,  16,     16,     4    ///< Standard 8-bit RGB tile 16x16
#define TILE_32x32_RGB8       NanoCanvas8,  32,     32,     5    ///< Standard 8-bit RGB tile 32x32
#define TILE_8x8_MONO_8       NanoCanvas1_8,8,      8,      3    ///< Standard 1-bit tile 8x8 for RGB mode
// Tiles for 16-bit displays
#define TILE_8x8_RGB16        NanoCanvas16, 8,      8,      3    ///< Standard 16-bit RGB tile 8x8
// Adafruit tiles
#define ADATILE_8x8_MONO      AdafruitCanvas1,  8,  8,      3    ///< Use Adafruit GFX implementation as NanoEngine canvas
#define ADATILE_8x8_RGB8      AdafruitCanvas8,  8,  8,      3    ///< Use Adafruit GFX implementation as NanoEngine canvas
#define ADATILE_8x8_RGB16     AdafruitCanvas16, 8,  8,      3    ///< Use Adafruit GFX implementation as NanoEngine canvas

/**
 * Type of user-specified draw callback.
 */
typedef bool (*TNanoEngineOnDraw)(void);

/**
 * This class template is responsible for holding and updating data about areas to be refreshed
 * on LCD display. It accepts canvas class, tile width in pixels, tile height in pixels and
 * number of bits in tile width as arguments for the template.
 * For example, for 8x8 8-bit RGB tiles the reference should be NanoEngineTiler<NanoCanvas8,8,8,3>,
 * and 3 bits means 3^2 = 8.
 * If you need to have single big buffer, holding the whole content for monochrome display,
 * you can specify something like this NanoEngineTiler<NanoCanvas1,128,64,7>.
 */
template<class C, lcduint_t W, lcduint_t H, uint8_t B>
class NanoEngineTiler
{
protected:
    /** Only child classes can initialize the engine */
    NanoEngineTiler()
    {
        refresh();
    };

public:
    /** Number of bits in tile size. 5 corresponds to 1<<5 = 32 tile size */
    static const uint8_t NE_TILE_SIZE_BITS = B;
    /** Width of tile in pixels */
    static const lcduint_t NE_TILE_WIDTH = W;
    /** Height of tile in pixels */
    static const lcduint_t NE_TILE_HEIGHT = H;
    /** Max tiles supported in X */
    static const uint8_t NE_MAX_TILES_NUM = 64 >> (B - 3);

    /** object, representing canvas. Use it in your draw handler */
    static C canvas;

    /**
     * Marks all tiles for update. Actual update will take place in display() method.
     */
    static void refresh()
    {
        memset(m_refreshFlags,0xFF,sizeof(uint16_t) * NanoEngineTiler<C,W,H,B>::NE_MAX_TILES_NUM);
    }

    /**
     * Mark specified area in pixels for redrawing by NanoEngine.
     * Actual update will take place in display() method.
     * @note assumes that rect is in local screen coordinates
     */
    static void refresh(const NanoRect &rect)
    {
        refresh(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
    }

    /**
     * Mark specified area in pixels for redrawing by NanoEngine.
     * Actual update will take place in display() method.
     */
    static void refresh(const NanoPoint &point)
    {
        if ((point.y<0) || ((point.y>>B)>=NE_MAX_TILES_NUM)) return;
        m_refreshFlags[(point.y>>B)] |= (1<<(point.x>>B));
    }

    /**
     * Mark specified area in pixels for redrawing by NanoEngine.
     * Actual update will take place in display() method.
     */
    static void refresh(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
    {
        if (y2 < 0) return;
        if (y1 < 0) y1 = 0;
        if (x1 < 0) x1 = 0;
        y1 = y1>>B;
        y2 = min((y2>>B), NE_MAX_TILES_NUM - 1);
        for (uint8_t y=y1; y<=y2; y++)
        {
            for(uint8_t x=x1>>B; x<=(x2>>B); x++)
            {
                m_refreshFlags[y] |= (1<<x);
            }
        }
    }

    /**
     * Marks for refresh lcd area, which corresponds to specified rectangle in
     * global (World) coordinates. If engine offset is (0,0), then this function
     * refreshes the same area as refresh().
     */
    static void refreshWorld(const NanoRect &rect)
    {
        refreshWorld(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
    }

    /**
     * Marks for refresh lcd area, which corresponds to specified rectangle in
     * global (World) coordinates. If engine offset is (0,0), then this function
     * refreshes the same area as refresh().
     */
    static void refreshWorld(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
    {
        refresh(x1 - offset.x, y1 - offset.y, x2 - offset.x, y2 - offset.y);
    }

    /**
     * Marks specified pixel area for redrawing by NanoEngine.
     * @param point point in global (World) coordinates
     */
    static void refreshWorld(const NanoPoint &point)
    {
        refresh( point - offset );
    }

    /**
     * Switches engine canvas to local coordinates system. This method can be useful
     * to ease up drawing of some static elements on lcd display.
     * @warning do not call twice subsequentally.
     */
    static void localCoordinates()
    {
        canvas.offset -= offset;
    }

    /**
     * Switches engine canvas to global (World) coordinates system. This method can be useful
     * to create screen moving animation.
     * @warning do not call twice subsequentally.
     */
    static void worldCoordinates()
    {
        canvas.offset += offset;
    }

    /**
     * Moves engine coordinate to new position (this sets World coordinates offset).
     */
    static void moveTo(const NanoPoint & position)
    {
        offset = position;
    }

    /**
     * Moves engine coordinate to new position and mark whole display for refresh
     * (this sets World coordinates offset).
     */
    static void moveToAndRefresh(const NanoPoint & position)
    {
        moveTo(position);
        refresh();
    }

    /**
     * Returns current World offset
     */
    const NanoPoint & getPosition() const
    {
        return offset;
    }

    /**
     * Sets user-defined draw callback. This callback will be called everytime, engine needs
     * to update display content. If callback returns false, engine will not update those area.
     * You always have a way to find out, which area is being updated by engine via
     * NanoEngine<>::canvas::getOffset() and NanoEngine<>::NE_TILE_SIZE.
     * @warning   By default canvas in the engine is initialized with local screen coordinates. So
     *            graphics object with [0,0] coordinates will be placed at topleft position on the
     *            display. But engine supports also global coordinates, in this case actual object
     *            position depends on current engine offset. Refer to worldCoordinates() and
     *            localCoordinates().
     * @param callback - user-defined draw callback.
     * @note you can change draw callback anytime you need.
     * @see worldCoordinates()
     * @see localCoordinates()
     */
    static void drawCallback(TNanoEngineOnDraw callback)
    {
        m_onDraw = callback;
    }

    /**
     * @brief Returns true if point is inside the rectangle area.
     * Returns true if point is inside the rectangle area.
     * @param p - point to check
     * @param rect - rectangle, describing the region to check with the point
     * @returns true if point is inside the rectangle area.
     */
    static bool collision(NanoPoint &p, NanoRect &rect) { return rect.collision( p ); }

protected:
    /**
     * Contains information on tiles to be updated.
     * Elements of array are rows and bits are columns.
     */
    static uint16_t   m_refreshFlags[NE_MAX_TILES_NUM];

    /** Callback to call if specific tile needs to be updated */
    static TNanoEngineOnDraw m_onDraw;

    /**
     * @brief refreshes content on oled display.
     * Refreshes content on oled display. Call it, if you want to update the screen.
     * Engine will update only those areas, which are marked by refresh()
     * methods.
     */
    static void displayBuffer();

    /**
     * @brief prints popup message over display content
     * prints popup message over display content
     * @param msg - message to display
     */
    static void displayPopup(const char *msg);
private:
    /** Buffer, used by NanoCanvas */
    static uint8_t    m_buffer[W * H * C::BITS_PER_PIXEL / 8];

    static NanoPoint offset;
};

template<class C, lcduint_t W, lcduint_t H, uint8_t B>
uint16_t NanoEngineTiler<C,W,H,B>::m_refreshFlags[NE_MAX_TILES_NUM];

template<class C, lcduint_t W, lcduint_t H, uint8_t B>
uint8_t NanoEngineTiler<C,W,H,B>::m_buffer[W * H * C::BITS_PER_PIXEL / 8];

template<class C, lcduint_t W, lcduint_t H, uint8_t B>
C NanoEngineTiler<C,W,H,B>::canvas(W, H, m_buffer);

template<class C, lcduint_t W, lcduint_t H, uint8_t B>
NanoPoint NanoEngineTiler<C,W,H,B>::offset = {0, 0};

template<class C, lcduint_t W, lcduint_t H, uint8_t B>
TNanoEngineOnDraw NanoEngineTiler<C,W,H,B>::m_onDraw = nullptr;

template<class C, lcduint_t W, lcduint_t H, uint8_t B>
void NanoEngineTiler<C,W,H,B>::displayBuffer()
{
    if (!m_onDraw)  // If onDraw handler is not set, just output current canvas
    {
        canvas.blt();
        return;
    }
    for (lcduint_t y = 0; y < ssd1306_lcd.height; y = y + NE_TILE_HEIGHT)
    {
        uint16_t flag = m_refreshFlags[y >> NE_TILE_SIZE_BITS];
        m_refreshFlags[y >> NE_TILE_SIZE_BITS] = 0;
        for (lcduint_t x = 0; x < ssd1306_lcd.width; x = x + NE_TILE_WIDTH)
        {
            if (flag & 0x01)
            {
                canvas.setOffset(x, y);
                if (m_onDraw())
                {
                    canvas.setOffset(x, y);
                    canvas.blt();
                }
            }
            flag >>=1;
        }
    }
}

template<class C, lcduint_t W, lcduint_t H, uint8_t B>
void NanoEngineTiler<C,W,H,B>::displayPopup(const char *msg)
{
    NanoRect rect = { {8, (ssd1306_lcd.height>>1) - 8}, {ssd1306_lcd.width - 8, (ssd1306_lcd.height>>1) + 8} };
    // TODO: It would be nice to calculate message height
    NanoPoint textPos = { (ssd1306_lcd.width - (lcdint_t)strlen(msg)*s_fixedFont.h.width) >> 1, (ssd1306_lcd.height>>1) - 4 };
    refresh(rect);
    for (lcduint_t y = 0; y < ssd1306_lcd.height; y = y + NE_TILE_HEIGHT)
    {
        uint16_t flag = m_refreshFlags[y >> NE_TILE_SIZE_BITS];
        m_refreshFlags[y >> NE_TILE_SIZE_BITS] = 0;
        for (lcduint_t x = 0; x < ssd1306_lcd.width; x = x + NE_TILE_WIDTH)
        {
            if (flag & 0x01)
            {
                canvas.setOffset(x, y);
                if (m_onDraw) m_onDraw();
                canvas.setOffset(x, y);
                canvas.setColor(RGB_COLOR8(0,0,0));
                canvas.fillRect(rect);
                canvas.setColor(RGB_COLOR8(192,192,192));
                canvas.drawRect(rect);
                canvas.printFixed( textPos.x, textPos.y, msg);

                canvas.blt();
            }
            flag >>=1;
        }
    }
}

/**
 * @}
 */

#endif

