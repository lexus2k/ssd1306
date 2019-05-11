/*
    MIT License

    Copyright (c) 2016-2019, Alexey Dynda

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

#define SSD1306_DIRECT_TEST(X)  X
#define SSD1306_GFX_TEST(X)     0
#define SSD1306_ENGINE_TEST(X)  0
#define ADAFRUIT_TEST(X)        0
#define U8G2_TEST(X)            0

#define TEST_I2C                0
#define TEST_SPI                1

#define OLED_SSD1306            0
#define OLED_SSD1351            1

#define TEST_INTERFACE          TEST_SPI
#define TEST_OLED               OLED_SSD1306




#if SSD1306_DIRECT_TEST(1) || SSD1306_ENGINE_TEST(1) || SSD1306_GFX_TEST(1)
#include "ssd1306.h"
#if SSD1306_GFX_TEST(1)
#include "nano_gfx.h"
#if TEST_OLED == OLED_SSD1306
uint8_t data[1024];
NanoCanvas display(128,64,data);
#endif
#if TEST_OLED == OLED_SSD1351
uint8_t data[128*128/8];
NanoCanvas display(128,128,data);
#endif
#endif
#if SSD1306_ENGINE_TEST(1)
#include "nano_engine.h"

#if TEST_OLED == OLED_SSD1306
uint8_t data[1024];
NanoCanvas1 display(128,64,data);
#endif
#if TEST_OLED == OLED_SSD1351
uint8_t data[128*128*16/8];
NanoCanvas16 display(128,128,data);
#endif
#endif

#elif ADAFRUIT_TEST(1)
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#if TEST_INTERFACE == TEST_SPI
Adafruit_SSD1306 display(5,3,4);
#elif TEST_INTERFACE == TEST_I2C
Adafruit_SSD1306 display(4);
#endif

#elif U8G2_TEST(1)
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#if TEST_INTERFACE == TEST_SPI
U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 4, /* dc=*/ 5, /* reset=*/ 3);
#elif TEST_INTERFACE == TEST_I2C
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
#endif

#endif

typedef struct
{
    const char *name;
    void (*run)(void);
    uint32_t result;
} TestInfo;

void benchmark(TestInfo *test)
{
    uint32_t start = micros();
    uint32_t end;
    for (uint8_t i=0; i<8; i++)
    {
        test->run();
    }
    end = micros();
    test->result = ((uint32_t)(end - start))/8;
}

const char clearScreen[] PROGMEM = "clearScreen";
const char drawRect[] PROGMEM = "drawRect";
const char sendBuffer[] PROGMEM = "sendBuffer";
const char drawText[] PROGMEM = "drawText";
const char drawText2x[] PROGMEM = "drawText 2x";


void doClearScreen()
{
    SSD1306_DIRECT_TEST  (ssd1306_clearScreen());
    SSD1306_GFX_TEST     (display.clear());
    SSD1306_ENGINE_TEST  (display.clear());
    ADAFRUIT_TEST        (display.clearDisplay());
    U8G2_TEST            (u8g2.clearBuffer());
}

void doSendBuffer()
{
    SSD1306_DIRECT_TEST  ();  // not applicable
    SSD1306_GFX_TEST     (display.blt(0,0));
    #if TEST_OLED == OLED_SSD1351
    SSD1306_ENGINE_TEST  (ssd1306_setMode( LCD_MODE_NORMAL ));
    #endif
    SSD1306_ENGINE_TEST  (display.blt());
    ADAFRUIT_TEST        (display.display());
    U8G2_TEST            (u8g2.sendBuffer());
}

void drawRect32x32()
{
    SSD1306_DIRECT_TEST  (ssd1306_drawRect(16,16,48,48));
    SSD1306_GFX_TEST     (display.drawRect(16,16,48,48));
    SSD1306_ENGINE_TEST  (display.drawRect(16,16,48,48));
    ADAFRUIT_TEST        (display.drawRect(16,16,32,32,WHITE));
    U8G2_TEST            (u8g2.drawFrame(16,16,32,32));
}

void drawTextHello()
{
    SSD1306_DIRECT_TEST  (ssd1306_printFixed(0,0,"Hello, world!", STYLE_NORMAL));
    SSD1306_GFX_TEST     (display.printFixed(0,0,"Hello, world!"));
    SSD1306_ENGINE_TEST  (display.printFixed(0,0,"Hello, world!"));
    ADAFRUIT_TEST        (display.setCursor(0,0);
                          display.print("Hello, world!"));
    U8G2_TEST            (u8g2.setFont(u8g2_font_5x8_tr);
                          u8g2.drawStr(0,10,"Hello, world!"));
}

void drawTextHello2x()
{
    SSD1306_DIRECT_TEST  (ssd1306_printFixedN(0,0,"Hello, world!", STYLE_NORMAL, FONT_SIZE_2X));
    SSD1306_GFX_TEST     (display.printFixed2x(0,0,"Hello, world!"));
    SSD1306_ENGINE_TEST  ();// not supported
    ADAFRUIT_TEST        (display.setTextSize(2);
                          display.setCursor(0,0);
                          display.print("Hello, world!"));
    U8G2_TEST            ();// not supported
}

TestInfo tests[] =
{
    {
        .name = clearScreen,
        .run = doClearScreen,
    },
    {
        .name = sendBuffer,
        .run = doSendBuffer,
    },
    {
        .name = drawRect,
        .run = drawRect32x32,
    },
    {
        .name = drawText2x,
        .run = drawTextHello2x,
    },
    {
        .name = drawText,
        .run = drawTextHello,
    },
};

void initLcd()
{
    #if (SSD1306_DIRECT_TEST(1)) || (SSD1306_ENGINE_TEST(1)) || (SSD1306_GFX_TEST(1))
    /* Replace the line below with ssd1306_128x32_i2c_init() if you need to use 128x32 display */
    #if (TEST_INTERFACE == TEST_SPI)
#if TEST_OLED == OLED_SSD1306
    ssd1306_128x64_spi_init(3,4,5);
#endif
#if TEST_OLED == OLED_SSD1351
    ssd1351_128x128_spi_init(24, 0, 23); // Use this line for Raspberry  (gpio24=RST, 0=CE, gpio23=D/C)
//    ssd1351_128x128_spi_init(3,4,5); // Use this line for Atmega328p
#endif
    #elif (TEST_INTERFACE == TEST_I2C)
    ssd1306_128x64_i2c_init();
    #endif
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    #endif
    ADAFRUIT_TEST        (display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
                          display.clearDisplay();
                          display.setTextSize(1);
                          display.setTextColor(WHITE));
    U8G2_TEST            (u8g2.begin();
                          u8g2.setFont(u8g2_font_5x8_tr));
}

void setup()
{
    #ifdef __linux__
    #else
    Serial.begin(115200);
    #endif
    initLcd();
    for (uint8_t i=0; i<sizeof(tests)/sizeof(TestInfo); i++)
    {
         SSD1306_DIRECT_TEST  (ssd1306_clearScreen());
         SSD1306_GFX_TEST     (ssd1306_clearScreen();
                               display.clear());
         SSD1306_ENGINE_TEST  (ssd1306_clearScreen();
                               display.clear());
         ADAFRUIT_TEST        (display.clearDisplay());
         U8G2_TEST            (u8g2.clearBuffer());
         benchmark(&tests[i]);
         const char *p = tests[i].name;
         while (pgm_read_byte(p))
         {
             #ifdef __linux__
             printf("%c", (char)pgm_read_byte(p++));
             #else
             Serial.print((char)pgm_read_byte(p++));
             #endif
         }
         #ifdef __linux__
         printf(": %u us\n", tests[i].result);
         #else
         Serial.print(": ");
         Serial.print(tests[i].result);
         Serial.println("us");
         #endif
         doSendBuffer();
         delay(2000);
    }

    doSendBuffer();
}


void loop()
{
}
