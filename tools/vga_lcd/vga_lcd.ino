#include "ssd1306.h"
#include "nano_engine.h"
#include "intf/ssd1306_interface.h"

#define DEBUG

void uart_send_byte(uint8_t c);

extern uint16_t ssd1306_color;

///////////////////////////////// VGA INTERFACE ///////////////////////////////////

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
    s_vga_command = 0xFF;
}

void vga_interface_close()
{
}

void vga_put_pixel4(uint8_t x, uint8_t y, uint8_t color)
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

void vga_interface_send_byte4(uint8_t data)
{
    if (s_vga_command == 0xFF)
    {
        s_vga_command = data;
        return;
    }
    if (s_vga_command == 0x40)
    {
        uint8_t color = ((data & 0x80) >> 5) | ((data & 0x10) >> 3) | ((data & 0x02)>>1);
        vga_put_pixel4(s_cursor_x, s_cursor_y, color);
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
    if (!s_vga_command)
    {
        s_vga_command = data;
    }
    // command mode
}

void vga_interface_send_byte1(uint8_t data)
{
    if (s_vga_command == 0xFF)
    {
        s_vga_command = data;
        return;
    }
    if (s_vga_command == 0x40)
    {
        uint8_t mask = (0x1 << (s_cursor_x & 0x7));
        uint16_t addr = (s_cursor_x + s_cursor_y*ssd1306_lcd.width)/8;
        for(uint8_t i=8; i>0; i++)
        {
            if (data & 0x01)
            {
                s_vga_buffer[addr] |= mask;
            }
            else
            {
                s_vga_buffer[addr] &= ~mask;
            }
            data>>=1;
            addr+=ssd1306_lcd.width/8;
        }
        s_cursor_x++;
        return;
    }
    // command mode
}

void vga_interface_send_bytes(const uint8_t *buffer, uint16_t len)
{
    while (len--)
    {
        ssd1306_intf.send(*buffer);
        buffer++;
    }
}

void init_vga_interface()
{
    ssd1306_intf.spi = 0;
    ssd1306_intf.start = vga_interface_init;
    ssd1306_intf.stop = vga_interface_stop;
    ssd1306_intf.send = vga_interface_send_byte4;
    ssd1306_intf.send_buffer = vga_interface_send_bytes;
    ssd1306_intf.close = vga_interface_close;
}

/////////////////////////////// VGA LCD //////////////////////////////////

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

/////////////////////////// UART /////////////////////////////////

#define BAUD 115200
#include <util/setbaud.h>
#define UART_BUFFER_RX  32  // :( Still need large buffer to process USART RX bytes
static uint8_t uart_get_ptr = 0;
volatile static uint8_t uart_put_ptr = 0;
volatile static uint8_t uart_buf[UART_BUFFER_RX];

ISR(USART_RX_vect, ISR_BLOCK)
{
//    if (bit_is_clear(UCSR0A, FE0)) // Do not perform error checks for now
//    {
        uart_buf[uart_put_ptr] = UDR0;
        uart_put_ptr = (uart_put_ptr+1) & (UART_BUFFER_RX - 1);
//    }
//    else
//    {
//        volatile unsigned char data __attribute__((unused)) = UDR0;
//    }
}

void uart_init()
{
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);    
}

void uart_send_byte(uint8_t c)
{
    loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}

uint8_t uart_byte_available(void)
{
    return uart_put_ptr != uart_get_ptr;
}

static inline uint8_t uart_byte_available_direct(void)
{
    return bit_is_set(UCSR0A, RXC0);
}

uint8_t uart_read_byte(void)
{
    uint8_t data = uart_buf[uart_get_ptr];
    uart_get_ptr = (uart_get_ptr + 1) & (UART_BUFFER_RX - 1);
    return data;
}

static inline uint8_t uart_read_byte_direct(void)
{
    return UDR0;
}

#ifdef DEBUG
void debug_print_vga_buffer()
{
    for(int y = 0; y < ssd1306_lcd.height; y++)
    {
        for(int x = 0; x < ssd1306_lcd.width; x++)
        {
            uint8_t color = (s_vga_buffer[(y*ssd1306_lcd.width + x)/2] >> ((x&1)<<2)) & 0x0F;
            if (color)
            {
                uart_send_byte('#');
                uart_send_byte('#');
            }
            else
            {
                uart_send_byte(' ');
                uart_send_byte(' ');
            }
        }
        uart_send_byte('\r');
        uart_send_byte('\n');
    }
    uart_send_byte('\r');
    uart_send_byte('\n');
}
#endif

enum
{
    UART_CMD_NONE      = 0x00, // Module is waiting for any input on UART
    UART_CMD_PROXY     = 0x01, // Module proxies all bytes to vga library interface
    UART_CMD_LISTEN    = 0x02, // Module waits for command over UART
    UART_CMD_ESCAPE    = 0x7D, // Escape symbol, next needs to be modified via ^0x20
    UART_CMD_FRAME_END = 0x7E, // Stop communication
};

/** Warning. May hold cpu while waiting for byte after 0x7D */
void vga_uart_on_receive(uint8_t data)
{
    static uint8_t s_uart_command = UART_CMD_NONE;
    static uint8_t s_uart_arg = 0;
    for(;;)
    {
        if (data == UART_CMD_FRAME_END)
        {
            if (s_uart_command != UART_CMD_NONE)
            {
                s_uart_command = UART_CMD_NONE;
                #ifdef DEBUG
                    debug_print_vga_buffer();
                #endif
            }
            return;
        }
        if (data == UART_CMD_ESCAPE)
        {
            while (!uart_byte_available());
            data = uart_read_byte() ^ 0x20;
        }
        break;
    }
    if (s_uart_command == UART_CMD_NONE)
    {
        if ((data == 0x40) || (data == 0x00))
        {
            // Proxy standard LCD controller commands vga interface
            ssd1306_intf.send(data);
            s_uart_command = UART_CMD_PROXY;
            return;
        }
        s_uart_command = data;
        s_uart_arg = 0;
    }
    if (s_uart_command == UART_CMD_PROXY)
    {
        ssd1306_intf.send(data);
        return;
    }
    // command mode
    if (s_uart_command == UART_CMD_LISTEN)
    {
        s_uart_command = data;
        s_uart_arg = 0;
    }
    if (s_uart_command == 'C')
    {
        ssd1306_clearScreen();
        ssd1306_setCursor(0,0);
        s_uart_command = UART_CMD_LISTEN;
    }
    if (s_uart_command == 'T')
    {
        if (s_uart_arg) ssd1306_write(data);
    }
    if (s_uart_command == 'M')
    {
        if (s_uart_arg)
        {
            if (data >= '0') data -= '0';
            ssd1306_setMode(data);
            s_uart_command = UART_CMD_LISTEN;
        }
    }
    s_uart_arg++;
}


/////////////////////////////// MAIN /////////////////////////////////////

//NanoEngine8 engine;

/*bool drawAll()
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
}*/

void setup()
{
    init_vga_interface();
    init_vga_lcd();
    #ifdef DEBUG
        uart_init();
//        Serial.begin(115200);
    #endif

    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen();
    ssd1306_printFixed(10,8,"SSD1306",STYLE_BOLD);
    ssd1306_printFixed(24,16,"by",STYLE_BOLD);
    ssd1306_printFixed(4,24,"Alexey D.",STYLE_BOLD);

/*    engine.begin();
    engine.drawCallback( drawAll );
    engine.refresh();
    engine.display(); */
    
    #ifdef DEBUG
        debug_print_vga_buffer();
    #endif
}

void loop()
{
    while (uart_byte_available())
    {
        uint8_t data = uart_read_byte();
        vga_uart_on_receive(data);
    }
}
