/*
    MIT License

    Copyright (c) 2016-2018, Alexey Dynda

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
 *   Attiny85 PINS
 *             ____
 *   RESET   -|_|  |- 3V
 *   SCL (3) -|    |- (2)
 *   SDA (4) -|    |- (1)
 *   GND     -|____|- (0)
 *
 *   Atmega328 PINS: connect LCD to A4/A5
 */

#include "ssd1306.h"

struct FontPair {
    const uint8_t* primaryFont;    // Primary font
    const uint8_t* secondaryFont;  // Secondary font (NULL if not applicable)
    bool isFreeFont;               // Fixed = 0, Free = 1
    const char* testString;        // Text to be displayed
};

FontPair fonts[14] = {
    {ssd1306xled_font6x8, NULL, 0,             // Standard ASCII 6x8 Fixed Type font
    "Hello, World! 12345 ~!@#$%^&*()"},
    {ssd1306xled_font8x16, NULL, 0,            // Standard ASCII 8x16 Fixed Type font
    "This is 8x16 ~!@#$%^"},
    {ssd1306xled_font6x8, ssd1306xled_font6x8_German, 0, // German chars for standard ASCII 6x8 Fixed Type font
    "Grüße, Ökonomisch! Äpfel & Straße"},
    {ssd1306xled_font6x8_AB, NULL, 0,          // Caps-only 6x8 Fixed Type font
    "ONLY CAPITAL LETTERS 6 BY 8"},
    {ssd1306xled_font5x7, NULL, 0,             // Standard ASCII 5x7 Fixed Type font
    "5x7 font test 1234 ~!@#"},
    {ssd1306xled_font5x7_AB, NULL, 0,          // Caps-only 5x7 Fixed Type font
    "5 BY 7 CAPITAL ONLY!"},
    {digital_font5x7_123, NULL, 0,             // Digits-only Digital ASCII 5x7 Fixed Type font
    "1234567890"},
    {digital_font5x7_AB, NULL, 0,              // Caps-only Digital 5x7
    "DIGITAL FONT WITH CAPS"},
    {digital_font5x7, NULL, 0,                 // Full Digital ASCII 5x7
    "Full digital font !@#$%^&*()"},
    {free_calibri11x12, NULL, 1,               // Calibri ASCII 11x12 Free Type font
    "Calibri Font Example Text"},
    {free_calibri11x12, free_calibri11x12_cyrillic, 1, // Calibri ASCII 11x12 Free Type with Cyrillic fallback
    "Привет, мир!"},
    {free_calibri11x12, free_calibri11x12_latin, 1,    // Calibri ASCII 11x12 Free Type with Latin extended fallback
    "Latin Version!"},
    {courier_new_font11x16_digits, NULL, 0,    // Courier New font 11x16 digits only
    "123456!@#$%"},
    {comic_sans_font24x32_123, NULL, 0,        // Comic Sans font 24x32 digits only
    "123!#$%"}
};

int currentFontIndex = 0; // Font cycling index

void setup() {
    // Initialize the SSD1306 OLED display for 128x64 resolution (I2C interface)
    ssd1306_128x64_i2c_init();

    // Clear the screen
    ssd1306_clearScreen();

    // Start Serial communication for debugging
    Serial.begin(115200);
    Serial.println("Starting font testing...");
}

void loop() {
    // Clear the screen
    ssd1306_clearScreen();

    // Display the current font index for debugging
    char indexBuffer[16];
    sprintf(indexBuffer, "Font index: %d", currentFontIndex);
    ssd1306_setFixedFont(ssd1306xled_font6x8); // Use a small fixed font for debugging
    ssd1306_printFixed(0, 0, indexBuffer, STYLE_NORMAL);

    // Set the primary font
    if (fonts[currentFontIndex].isFreeFont) {
        ssd1306_setFreeFont(fonts[currentFontIndex].primaryFont);
    } else {
        ssd1306_setFixedFont(fonts[currentFontIndex].primaryFont);
    }

    // Set the secondary font (if applicable)
    if (fonts[currentFontIndex].secondaryFont != NULL) {
        ssd1306_setSecondaryFont(fonts[currentFontIndex].secondaryFont);
        Serial.println("Secondary font enabled for this configuration...");
    } else {
        ssd1306_setSecondaryFont(NULL); // Disable secondary font
    }

    // Print the test string for the current font configuration
    ssd1306_printFixed(0, 16, fonts[currentFontIndex].testString, STYLE_NORMAL);

    // Delay for viewing the font rendering
    delay(1500);

    // Clear the screen
    ssd1306_fillScreen(0x00);

    // Move to the next font
    currentFontIndex++;
    Serial.print("Switching to font index: ");
    Serial.println(currentFontIndex);

    // Reset the font index if we've reached the end
    if (currentFontIndex >= sizeof(fonts)/sizeof(fonts[0])) {
        currentFontIndex = 0;
    }
}
