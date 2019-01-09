# Using NanoEngine for systems with low resources

***

[tocstart]: # (toc start)

  * [Introduction](#introduction)
  * [Main idea of NanoEngine](#main-idea-of-nanoengine)
  * [Simple NanoEngine demo](#simple-nanoengine-demo)
  * [Reading keys with NanoEngine](#reading-keys-with-nanoengine)
  * [Draw monochrome bitmap](#draw-monochrome-bitmap)
  * [Draw moving bitmap](#draw-moving-bitmap)
  * [What if not to use draw callbacks](#what-if-not-to-use-draw-callbacks)
  * [Using Adafruit GFX with NanoEngine](#using-adafruit-gfx-with-nanoengine)
  * [To upper level](@ref index)

[tocend]: # (toc end)

<a name="introduction"></a>
## Introduction

Many applications use double-buffered output to physical display to avoid flickering effect, when a user observed non-completed picture for a short time. When working with color OLED/LCD displays like ssd1331, some micro-controllers do not have enough RAM to fit display buffer in. For example, in 8-bit mode ssd1331 OLED display needs 6144 bytes (`96*64`). But simple micro-controller like Atmega328 has only 2KiB, and Attiny85 has only 512B of RAM. But with ssd1306 library you still can create applications for those ones with good graphics.
ssd1306 library represents NanoEngine8, intended to be used with color OLED displays. Digit 8 means that engine implements 8-bit color mode. Refer to arkanoid8 as example, which can be run even on small Attiny85 with color ssd1331 display. The system supports NanoEngine1 for monochrome OLED displays, NanoEngine8 for 8-bit RGB OLED displays, NanoEngine16 for 16-bit RGB OLED displays.

<a name="main-idea-of-nanoengine"></a>
## Main idea of NanoEngine

There are 2 issues with tiny controllers:
  * they have a little RAM
  * they support low frequencies

The first problem is solved in NanoEngine by using double-buffer to redraw only part of display content at once. By default NanoEngine uses 8x8 small buffer (64 bytes) and 24 bytes to store information on areas, which need to be refreshed.
The second problem is solved almost the same way: refresh only those part of display content, which were changed since last frame update. For example, ssd1331 oled display work on SPI at 8MHz frequency, that means in ideal conditions the screen content can be refreshed 162 times per second (`8000000/(96*64*8)`). But with the data, you need to send also commands to the display and to do some other stuff. And real tests with Atmega328p show that `ssd1306_clearScreen()` can run only at 58 FPS, coping data from buffer to OLED memory runs slower. 
There is no such issue for Arduboy, since it uses monochrome OLED ssd1306 with only 1KiB of RAM buffer, and theoretical fps can be up to 976. For color display and small controllers the main solution is to refresh only part of display content. Arkanoid8 can give easily 60 fps with NanoEngine8

<a name="simple-nanoengine-demo"></a>
## Simple NanoEngine demo

```cpp
#include "ssd1306.h"
#include "nano_engine.h"

NanoEngine8 engine;

bool drawAll()
{
    engine.canvas.clear();
    engine.canvas.setColor(RGB_COLOR8(255,255,0));
    engine.canvas.drawRect(15,12,70,55);
    return true;   // if to return false, the engine will skip this part of screen update
}

void setup()
{
    /* Init SPI 96x64 RBG oled. 3 - RESET, 4 - CS (can be omitted, oled CS must be pulled down), 5 - D/C */
    ssd1331_96x64_spi_init(3, 4, 5);

    engine.begin();
    engine.setFrameRate(30);
    /* Set callback to draw parts, when NanoEngine8 asks */
    engine.drawCallback( drawAll );
}

void loop()
{
    if (!engine.nextFrame()) return;
    engine.refresh();  // Makes engine to refresh whole display content
    engine.display();
}
```

<a name="reading-keys-with-nanoengine"></a>
## Reading keys with NanoEngine

What, if we want to move yellow rectangle. There is easy way to do this with NanoEngine8. The engine supports up-to 6 keys: `BUTTON_DOWN`, `BUTTON_LEFT`, `BUTTON_RIGHT`, `BUTTON_UP`, `BUTTON_A`, `BUTTON_B`. All you need is to say the engine how it can get buttons state on your board. There are already 2 built-in implementations: `connectArduboyKeys()` allows using Arduboy platform hardware (remember that you need to replace ssd1306 oled with color ssd1331 oled), and `connectZKeypad()` allows using standard 5-keys Z-keypad (you can find it on E-bay). Or you can set custom key processing handler via `connectCustomKeys()` and implement your own hardware driver.

Example of using Z-keypad to move rectangle.
```cpp
#include "ssd1306.h"
#include "nano_engine.h"

NanoEngine8 engine;

NanoRect rect = { {15,12}, {60,35} }; // Lets make rect smaller than in previous example

bool drawAll()
{
    engine.canvas.clear();
    engine.canvas.setColor(RGB_COLOR8(255,255,0));
    engine.canvas.drawRect(rect);   // draw rect in buffer
    return true;
}

void setup()
{
    /* Init SPI 96x64 RBG oled. 3 - RESET, 4 - CS (can be omitted, oled CS must be pulled down), 5 - D/C */
    ssd1331_96x64_spi_init(3, 4, 5);

    engine.begin();
    engine.setFrameRate(30);
    engine.drawCallback( drawAll );  // Set callback to draw parts, when NanoEngine8 asks
    engine.connectZKeypad(0);        // Connect ADC-buttons Z-keypad to analog A0 pin
    engine.refresh();                // Makes engine to refresh whole display content at start-up
}

void loop()
{
    if (!engine.nextFrame()) return;
    NanoPoint point = {0,0};
    if (engine.pressed( BUTTON_RIGHT )) point.x = +1;
    if (engine.pressed( BUTTON_LEFT ))  point.x = -1;
    if (engine.pressed( BUTTON_UP ))    point.y = -1;
    if (engine.pressed( BUTTON_DOWN ))  point.y = +1;
    engine.refresh(rect);            // Update screen content at old rect position
    rect += point;                   // Move rect according to pressed keys
    engine.refresh(rect);            // Update screen content at new rect position
    engine.display();                // refresh display content
}
```

<a name="draw-monochrome-bitmap"></a>
## Draw monochrome bitmap

```cpp
#include "ssd1306.h"
#include "nano_engine.h"

NanoEngine8 engine;

const uint8_t heartSprite[8] PROGMEM =
{
    0B00001110,
    0B00011111,
    0B00111111,
    0B01111110,
    0B01111110,
    0B00111101,
    0B00011001,
    0B00001110
};

bool drawAll()
{
    engine.canvas.clear();
    engine.canvas.setMode(0);  // We want to draw non-transparent bitmap
    engine.canvas.setColor(RGB_COLOR8(255,0,0));  // draw with red color
    engine.canvas.drawBitmap1(10, 20, 8, 8, heartSprite);
    return true;
}

void setup()
{
    /* Init SPI 96x64 RBG oled. 3 - RESET, 4 - CS (can be omitted, oled CS must be pulled down), 5 - D/C */
    ssd1331_96x64_spi_init(3, 4, 5);
    engine.begin();
    engine.drawCallback( drawAll );  // Set callback to draw parts, when NanoEngine8 asks
    engine.refresh();                // Makes engine to refresh whole display content at start-up
}

void loop()
{
    if (!engine.nextFrame()) return;
    engine.display();                // refresh display content
}
```

<a name="draw-moving-bitmap"></a>
## Draw moving bitmap

In some applications like games, there is need to move bitmaps. It is easy to do this with ssd1306 library using NanoSprite objects. NanoSprite object is responsible for refreshing areas, touched by sprite. So, you need only to move sprite, where you want, the engine will take care of updating display content.

```cpp
#include "ssd1306.h"
#include "nano_engine.h"

const uint8_t heartSprite[8] PROGMEM =
{
    0B00001110,
    0B00011111,
    0B00111111,
    0B01111110,
    0B01111110,
    0B00111101,
    0B00011001,
    0B00001110
};

NanoEngine8 engine;
NanoSprite<NanoEngine8, engine> sprite( {0, 0}, {8, 8}, heartSprite );

bool drawAll()
{
    engine.canvas.clear();
    engine.canvas.setMode(0);  // We want to draw non-transparent bitmap
    engine.canvas.setColor(RGB_COLOR8(255,0,0));  // draw with red color
    sprite.draw();
    return true;
}

void setup()
{
    /* Init SPI 96x64 RBG oled. 3 - RESET, 4 - CS (can be omitted, oled CS must be pulled down), 5 - D/C */
    ssd1331_96x64_spi_init(3, 4, 5);
    engine.begin();
    engine.drawCallback( drawAll );  // Set callback to draw parts, when NanoEngine8 asks
    engine.refresh();                // Makes engine to refresh whole display content at start-up
}

void loop()
{
    if (!engine.nextFrame()) return;
    // You will see horizontal flying heart
    sprite.moveBy( { 1, 0 } );
    engine.display();                // refresh display content
}
```

<a name="what-if-not-to-use-draw-callbacks"></a>
## What if not to use draw callbacks

If you don't want to use draw callbacks in your application, but still need a power of NanoEngine, then there is one way for you: to use full-screen double-buffering with NanoEngine. The example, you will find below, shows how to use full-screen double buffering for monochrome 128x64 ssd1306 oled display. This example can be run on Atmega328p and more powerful micro controllers. It clears back-buffer every time engine says to redraw the frame. But you can preserve previously prepared image by removing call to `engine.canvas.clear()`.

```cpp
#include "ssd1306.h"
#include "nano_engine.h"

NanoEngine<BUFFER_128x64_MONO> engine;

void setup()
{
    // Init SPI 128x64 monochrome oled.
    // 3 - RESET, 4 - CS (can be omitted, oled CS must be pulled down), 5 - D/C
    ssd1306_128x64_spi_init(3, 4, 5);

    engine.begin();
    engine.setFrameRate(30);
}

void loop()
{
    if (!engine.nextFrame()) return;
    engine.canvas.clear();    // This step can be removed, if you don't want to clear buffer
    engine.canvas.drawRect(15,12,70,55);
    engine.display();
}
```

<a name="using-adafruit-gfx-with-nanoengine"></a>
## Using Adafruit GFX with NanoEngine

Many developers are familiar with nice AdafruitGFX library. It provides rich set of graphics functions. Starting with 1.7.0 ssd1306 library it is possible to use AdafruiGFX api in combination with NanoEngine. And it is really easy.
You need to remember only, that AdafruitGFX uses different approach, when working with rectangles and monochrome images. NanoCanvas expects monochrome bitmaps in native ssd1306 format, while Adafruit uses more native format for human. If you compare an example below with examples above you will understand, what's the difference (heartImage). Refer to AdafruitGFX documentation.

```cpp
// Define this before including library header, this will give Adafruit GFX support
// !!! Don't forget to install AdafruitGFX library to your Arduino IDE !!!
#define CONFIG_ADAFRUIT_GFX_ENABLE

#include "ssd1306.h"
#include "nano_engine.h"

// Now you can use AdafruitGFX by referencing engine.canvas
NanoEngine<ADATILE_8x8_RGB8> engine;

const PROGMEM uint8_t heartImage[8] =
{
    0B01100110,
    0B11111001,
    0B11111101,
    0B11111111,
    0B01111110,
    0B00111100,
    0B00011000,
    0B00000000
};

bool drawAll()
{
    engine.canvas.fillScreen( 0 );
    engine.canvas.drawBitmap(10, 20, heartSprite, 8, 8, RGB_COLOR8(255,0,0)); // draw bitmap with red color
    return true;
}

void setup()
{
    /* Init SPI 96x64 RBG oled. 3 - RESET, 4 - CS (can be omitted, oled CS must be pulled down), 5 - D/C */
    ssd1331_96x64_spi_init(3, 4, 5);
    engine.begin();
    engine.drawCallback( drawAll );  // Set callback to draw parts, when NanoEngine8 asks
    engine.refresh();                // Makes engine to refresh whole display content at start-up
}

void loop()
{
    if (!engine.nextFrame()) return;
    engine.display();                // refresh display content
}
```

