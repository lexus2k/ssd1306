#include "ssd1306.h"
#include "nano_engine.h"
#include "intf/ssd1306_interface.h"

#define DEBUG

extern uint16_t ssd1306_color;
static uint8_t s_vga_command = 0xFF;
static uint8_t s_cursor_x = 0;
static uint8_t s_cursor_y = 0;
static uint8_t s_column = 0;
static uint8_t s_column_end = 0;
static uint8_t s_page = 0;
static uint8_t s_vga_buffer[64*40/2] = {0};

void vga_interface_init()
{
    s_vga_command = 0xFF;
}

void vga_interface_stop()
{
}

void vga_interface_close()
{
}

void vga_put_pixel(uint8_t x, uint8_t y, uint8_t color)
{
    uint8_t mask;
    if (x&1)
    {
        mask = 0x0F;
        color <<= 4;
    }
    else
    {
        mask = 0xF0;
    }
    uint16_t addr = (x + y*ssd1306_lcd.width)/2;
    s_vga_buffer[addr] &= mask;
    s_vga_buffer[addr] |= color;
}

void vga_interface_send_byte(uint8_t data)
{
    if (s_vga_command == 0xFF)
    {
        s_vga_command = data;
        return;
    }
    if (s_vga_command == 0x40)
    {
        uint8_t color = ((data & 0x80) >> 5) | ((data & 0x10) >> 3) | ((data & 0x02)>>1);
        vga_put_pixel(s_cursor_x, s_cursor_y, color);
        if (s_page == 0xFF)
        {
            s_cursor_x++;
            if (s_cursor_x > s_column_end)
            {
                s_cursor_x = s_column;
                s_cursor_y++;
            }
        }
        else
        {
            s_cursor_y++;
            if ((s_cursor_y & 0x07) == 0)
            {
                s_cursor_y = s_page * 8;
                s_cursor_x++;
            }
        }
        return;
    }
    // command mode
}

void vga_interface_send_bytes(const uint8_t *buffer, uint16_t len)
{
    while (len--)
    {
        vga_interface_send_byte(*buffer);
        buffer++;
    }
}

void init_vga_interface()
{
    ssd1306_intf.spi = 0;
    ssd1306_intf.start = vga_interface_init;
    ssd1306_intf.stop = vga_interface_stop;
    ssd1306_intf.send = vga_interface_send_byte;
    ssd1306_intf.send_buffer = vga_interface_send_bytes;
    ssd1306_intf.close = vga_interface_close;
}

void vga_set_block1(lcduint_t x, lcduint_t y, lcduint_t w)
{
    uint8_t rx = w ? (x + w - 1) : (ssd1306_lcd.width - 1);
    s_column = x;
    s_column_end = rx;
    s_page = y;
    s_cursor_x = s_column;
    s_cursor_y = y*8;
    ssd1306_intf.start();
    ssd1306_intf.send(0x40);
}

void vga_next_page1(void)
{
    ssd1306_intf.stop();
    vga_set_block1(s_column,s_page+1,0);
}

void vga_set_block2(lcduint_t x, lcduint_t y, lcduint_t w)
{
    uint8_t rx = w ? (x + w - 1) : (ssd1306_lcd.width - 1);
    s_column = x;
    s_column_end = rx;
    s_page = 0xFF; // disable ssd1306 mode
    s_cursor_x = s_column;
    s_cursor_y = y;
    ssd1306_intf.start();
    ssd1306_intf.send(0x40);
}

void vga_next_page2(void)
{
}

void vga_send_pixels(uint8_t data)
{
    for (uint8_t i=8; i>0; i--)
    {
        if ( data & 0x01 )
        {
            ssd1306_intf.send( (uint8_t)ssd1306_color );
        }
        else
        {
            ssd1306_intf.send( 0B00000000 );
        }
        data >>= 1;
    }
}

void vga_send_pixels_buffer(const uint8_t *buffer, uint16_t len)
{
    while(len--)
    {
        vga_send_pixels(*buffer);
        buffer++;
    }
}

void vga_set_mode(lcd_mode_t mode)
{
    if (mode == LCD_MODE_NORMAL)
    {
        ssd1306_lcd.set_block = vga_set_block2;
        ssd1306_lcd.next_page = vga_next_page2;
    }
    else if (mode == LCD_MODE_SSD1306_COMPAT)
    {
        ssd1306_lcd.set_block = vga_set_block1;
        ssd1306_lcd.next_page = vga_next_page1;
    }
    // empty for a while
}

void init_vga_lcd()
{
    ssd1306_lcd.type = LCD_TYPE_SSD1331;
    ssd1306_lcd.width = 64;
    ssd1306_lcd.height = 40;
    ssd1306_lcd.set_block = vga_set_block1;
    ssd1306_lcd.next_page = vga_next_page1;
    ssd1306_lcd.send_pixels1  = vga_send_pixels;
    ssd1306_lcd.send_pixels_buffer1 = vga_send_pixels_buffer;
    ssd1306_lcd.send_pixels8 = ssd1306_intf.send;
    ssd1306_lcd.set_mode = vga_set_mode;
}

#ifdef DEBUG
void debug_print_vga_buffer()
{
    for(int y = 0; y < ssd1306_lcd.height; y++)
    {
        for(int x = 0; x < ssd1306_lcd.width; x++)
        {
            uint8_t color = (s_vga_buffer[(y*ssd1306_lcd.width + x)/2] >> ((x&1)<<2)) & 0x0F;
            if (color) Serial.print("##");
            else       Serial.print("  ");
        }
        Serial.println();
    }
    Serial.println();
}
#endif

NanoEngine8 engine;

bool drawAll()
{
    engine.canvas.clear();
    engine.canvas.setMode(CANVAS_MODE_TRANSPARENT);
    engine.canvas.setColor(RGB_COLOR8(255,255,0));
    engine.canvas.drawRect(11,12,60,32);
    engine.canvas.setColor(RGB_COLOR8(64,64,64));
    engine.canvas.fillRect(12,13,59,31);
    engine.canvas.setColor(RGB_COLOR8(255,0,0));
    engine.canvas.printFixed(16, 15, "This is example");
    return true;
}

void setup()
{
    init_vga_interface();
    init_vga_lcd();
    #ifdef DEBUG
        Serial.begin(115200);
    #endif

    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen();
    ssd1306_printFixed(1,8,"Hello",STYLE_BOLD);

    engine.begin();
    engine.drawCallback( drawAll );
    engine.refresh();
    engine.display();
    
    #ifdef DEBUG
        debug_print_vga_buffer();
    #endif
}

void loop()
{
}
