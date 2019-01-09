/*
 * Credits to https://github.com/bitluni/ESP32CompositeVideo
 */

#pragma once

#include "ssd1306_hal/io.h"

#if defined(CONFIG_VGA_AVAILABLE) && defined(CONFIG_VGA_ENABLE) && defined(__XTENSA__)

#include <stdint.h>
#include "driver/i2s.h"

typedef struct
{
  float lineMicros;
  float syncMicros;
  float blankEndMicros;
  float backMicros;
  float shortVSyncMicros;
  float overscanLeftMicros;
  float overscanRightMicros;
  float syncVolts;
  float blankVolts;
  float blackVolts;
  float whiteVolts;
  short lines;
  short linesFirstTop;
  short linesOverscanTop;
  short linesOverscanBottom;
  float imageAspect;
} TechProperties;

class CompositeOutput
{
public:

    enum Mode
    {
        PAL,
        NTSC
    };

    CompositeOutput(Mode mode, double Vcc = 3.3);

    void init(int xres, int yres, int bpp);

    void fillValues(uint8_t value, int count);

    void sendFrameHalfResolution(const uint8_t *frame);

private:
    const TechProperties &properties;

    int m_samples_per_line = 0;
    int samplesSync = 0;
    int samplesBlank = 0;
    int samplesBack = 0;
    int samplesActive = 0;
    int samplesBlackLeft = 0;
    int samplesBlackRight = 0;

    int samplesVSyncShort = 0;
    int samplesVSyncLong = 0;

    uint8_t levelSync = 0;
    uint8_t levelBlank = 0;
    uint8_t levelBlack = 0;
    uint8_t levelWhite = 0;
    uint8_t grayValues = 0;

    int targetXres = 0;
    int targetYres = 0;
    int targetYresEven = 0;
    int targetYresOdd = 0;

    int linesEven = 0;
    int linesOdd = 0;
    int linesEvenActive = 0;
    int linesOddActive = 0;
    int linesEvenVisible = 0;
    int linesOddVisible = 0;
    int linesEvenBlankTop = 0;
    int linesEvenBlankBottom = 0;
    int linesOddBlankTop = 0;
    int linesOddBlankBottom = 0;

    int m_buffer_width = 0;
    int m_buffer_height = 0;
    int m_bpp = 0;

//    float pixelAspect;

    uint16_t *line = nullptr;
    uint16_t *m_end = nullptr;
    uint16_t *m_ptr = nullptr;

    void init_hardware();
    void check_buffer();
    void generate_vsync();
    void generate_long_sync();
    void generate_short_sync();
    void generate_long_short_sync();
    void generate_short_long_sync();
    void generate_short_blank_sync();
    void generate_blank_short_sync();
    void generate_blank_line();
    const uint8_t * generate_line_from_buffer(const uint8_t * buffer);
};

#endif

