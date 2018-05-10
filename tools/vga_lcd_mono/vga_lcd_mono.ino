#include "ssd1306.h"
#include "nano_engine.h"
#include "intf/ssd1306_interface.h"

//#define DEBUG

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

void vga_put_pixel1(uint8_t x, uint8_t y, uint8_t color)
{
    uint8_t mask = 1 << (x & 0x7);
    uint16_t addr = (x + y*ssd1306_lcd.width)/8;
    if (color)
        s_vga_buffer[addr] |= mask;
    else
        s_vga_buffer[addr] &= ~mask;
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
        vga_put_pixel1(s_cursor_x, s_cursor_y+0, data & 0x1);
        vga_put_pixel1(s_cursor_x, s_cursor_y+1, data & 0x2);
        vga_put_pixel1(s_cursor_x, s_cursor_y+2, data & 0x4);
        vga_put_pixel1(s_cursor_x, s_cursor_y+3, data & 0x8);
        vga_put_pixel1(s_cursor_x, s_cursor_y+4, data & 0x10);
        vga_put_pixel1(s_cursor_x, s_cursor_y+5, data & 0x20);
        vga_put_pixel1(s_cursor_x, s_cursor_y+6, data & 0x40);
        vga_put_pixel1(s_cursor_x, s_cursor_y+7, data & 0x80);
        s_cursor_x++;
        return;
    }
    if (!s_vga_command)
    {
        s_vga_command = data;
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
    ssd1306_intf.send = vga_interface_send_byte1;
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

void vga_send_pixels_buffer(const uint8_t *buffer, uint16_t len)
{
    while(len--)
    {
        ssd1306_intf.send(*buffer);
        buffer++;
    }
}

void vga_set_mode(lcd_mode_t mode)
{
    if (mode == LCD_MODE_SSD1306_COMPAT)
    {
        ssd1306_lcd.set_block = vga_set_block1;
        ssd1306_lcd.next_page = vga_next_page1;
    }
    // empty for a while
}

void init_vga_lcd()
{
    ssd1306_lcd.type = LCD_TYPE_SSD1331;
    ssd1306_lcd.width = 96;
    ssd1306_lcd.height = 64;
    ssd1306_lcd.set_block = vga_set_block1;
    ssd1306_lcd.next_page = vga_next_page1;
    ssd1306_lcd.send_pixels1  = ssd1306_intf.send;
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
static uint8_t s_uart_interrupt = 0;

ISR(USART_RX_vect, ISR_BLOCK)
{
    if (bit_is_clear(UCSR0A, FE0)) // Do not perform error checks for now
    {
        uart_buf[uart_put_ptr] = UDR0;
        uart_put_ptr = (uart_put_ptr+1) & (UART_BUFFER_RX - 1);
    }
    else
    {
        volatile unsigned char data __attribute__((unused)) = UDR0;
    }
}

void uart_init(uint8_t interrupt)
{
    s_uart_interrupt = interrupt;
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | (s_uart_interrupt ? _BV(RXCIE0) : 0);
}

void uart_send_byte(uint8_t c)
{
    loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}

static inline uint8_t uart_byte_available_direct(void)
{
    return bit_is_set(UCSR0A, RXC0);
}

uint8_t uart_byte_available(void)
{
    if (s_uart_interrupt) return uart_put_ptr != uart_get_ptr;
    return uart_byte_available_direct();
}

static inline uint8_t uart_read_byte_direct(void)
{
    return UDR0;
}

uint8_t uart_read_byte(void)
{
    if (s_uart_interrupt)
    {
        uint8_t data = uart_buf[uart_get_ptr];
        uart_get_ptr = (uart_get_ptr + 1) & (UART_BUFFER_RX - 1);
        return data;
    }
    return uart_read_byte_direct();
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

////////////////////////////// VGA CRT controller /////////////////////////////

static const uint8_t H_SYNC_PIN = 3;
static const uint8_t V_SYNC_PIN = 10;
// Total number of lines used in specific scan mode
static uint16_t s_total_mode_lines = 64*4;

// Lines to skip before starting to draw first line of the screen content
// This includes V-sync signal + front porch
static const uint8_t V_BACKPORCH_LINES = 40;

// Lines to skip before starting to draw first line of the screen content
// This includes V-sync signal + front porch
volatile int s_current_scan_line;
volatile uint8_t s_lines_to_skip;
volatile uint8_t *s_current_scan_line_data = &s_vga_buffer[0];

// ISR: Vsync pulse
ISR(TIMER1_OVF_vect)
{
    s_current_scan_line = 0;
    s_current_scan_line_data = &s_vga_buffer[0];
    s_lines_to_skip = V_BACKPORCH_LINES;
} // end of TIMER1_OVF_vect

ISR(TIMER2_COMPB_vect) // for end of h-sync pulse
//ISR(TIMER2_OVF_vect) // for start of h-sync pulse
{
    // Implement all code directly in ISR: No any calls to subroutines!!!!
    // ISR should work as fast as possible
    if (s_lines_to_skip)
    {
        s_lines_to_skip--;
        return;
    }
    else if (s_current_scan_line >= s_total_mode_lines)
    {
        return;
    }
    // This is dejitter code, it purpose to start pixels output at the same offset after h-sync
    asm volatile(
      "     lds r16, %[timer0]    \n\t" //
      "     subi r16, -2           \n\t" // some offset, calculated experimentally
      "     andi r16, 7           \n\t" // use module 8 value from Timer 0 counter
      "     ldi r31, pm_hi8(LW)   \n\t" // load label address
      "     ldi r30, pm_lo8(LW)   \n\t" //
      "     add r30, r16          \n\t" // no need to multiply by 2 since AVR addresses are half-values
      "     adc r31, __zero_reg__ \n\t" //
      "     ijmp                  \n\t" //
      "LW:                        \n\t" //
      "     nop                   \n\t" //
      "     nop                   \n\t" //
      "     nop                   \n\t" //
      "     nop                   \n\t" //
      "     nop                   \n\t" //
      "     nop                   \n\t" //
      "     nop                   \n\t" //
    :
    : [timer0] "i" (&TCNT0)
    : "r30", "r31", "r16", "r17");
    // output all pixels
    asm volatile(
         "ldi r20, 128\n\t"
         ".rept 12\n\t"
         "ld r16, Z+\n\t"
         "out %[port], r16\n\t"
         "mul r16, r20\n\t"
         "out %[port], r1\n\t"
         "mul r1, r20\n\t"
         "out %[port], r1\n\t"
         "mul r1, r20\n\t"
         "out %[port], r1\n\t"
         "mul r1, r20\n\t"
         "out %[port], r1\n\t"
         "mul r1, r20\n\t"
         "out %[port], r1\n\t"
         "mul r1, r20\n\t"
         "out %[port], r1\n\t"
         "mul r1, r20\n\t"
         "out %[port], r1\n\t"
         ".endr\n\t"
         "nop\n\t"
         "ldi r16,0\n\t"
         "out %[port], r16 \n\t"
         "nop\n\t"
    :
    : [port] "I" (_SFR_IO_ADDR(PORTC)),
       "z" "I" ((uint8_t *)s_current_scan_line_data )
    : "r16", "r17", "r20", "r21", "memory"
    );

    s_current_scan_line++;
    if ((s_current_scan_line & 0x3) == 0)
    {
        s_current_scan_line_data+=96/8;
    }
}

void init_vga_crt_driver(void)
{
    cli();

    // Configure Timer 0 to calculate jitter fix
    TIMSK0=0;
    TCCR0A=0;
    TCCR0B=1;
    OCR0A=0;
    OCR0B=0;
    TCNT0=0;

    // Timer 1 - vertical sync pulses
    pinMode (V_SYNC_PIN, OUTPUT);
    TCCR1A=bit(WGM10) | bit(WGM11) | bit(COM1B1);
    TCCR1B=bit(WGM12) | bit(WGM13) | bit(CS12) | bit(CS10); //1024 prescaler
    OCR1A = 259;  // 16666 / 64 us = 260 (less one)
    OCR1B = 0;    // 64 / 64 us = 1 (less one)
    TIFR1 = bit (TOV1);   // clear overflow flag
    TIMSK1 = bit (TOIE1);  // interrupt on overflow on timer 1

    // Timer 2 - horizontal sync pulses
    pinMode (H_SYNC_PIN, OUTPUT);
    TCCR2A=bit(WGM20) | bit(WGM21) | bit(COM2B1); //pin3=COM2B1
    TCCR2B=bit(WGM22) | bit(CS21); //8 prescaler
    OCR2A = 63;   // 32 / 0.5 us = 64 (less one)
    OCR2B = 7;    // 4 / 0.5 us = 8 (less one)
//    TIFR2 = bit (TOV2);    // int on start of h-sync pulse
//    TIMSK2 = bit (TOIE2);  // int on start of h-sync pulse
    TIFR2 = bit (OCF2B);   // on end of h-sync pulse
    TIMSK2 = bit (OCIE2B); // on end of h-sync pulse

  // Set up USART in SPI mode (MSPIM)

  pinMode(14, OUTPUT);
//  pinMode(15, OUTPUT);
//  pinMode(16, OUTPUT);
  PORTC = 0;

  // prepare to sleep between horizontal sync pulses
//  set_sleep_mode (SLEEP_MODE_IDLE);
   sei();
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
    uart_init(0);
//        Serial.begin(115200);

    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen();
    ssd1306_printFixed(10,8,"SSD1306",STYLE_BOLD);
    ssd1306_printFixed(24,16,"by",STYLE_BOLD);
    ssd1306_printFixed(4,24,"Alexey D.",STYLE_BOLD);
    ssd1331_setColor(RGB_COLOR8(255,0,0));
    ssd1306_drawRect(0,0,95,63);

/*    engine.begin();
    engine.drawCallback( drawAll );
    engine.refresh();
    engine.display(); */
    
    #ifdef DEBUG
        debug_print_vga_buffer();
    #else
        init_vga_crt_driver();
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
