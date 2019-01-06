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

  static const i2s_port_t I2S_PORT = (i2s_port_t)I2S_NUM_0;

  enum Mode
  {
    PAL,
    NTSC
  };

  CompositeOutput(Mode mode, int xres, int yres, double Vcc = 3.3);

  void init();

  void fillValues(uint8_t value, int count);

  void sendFrameHalfResolution(const uint8_t *frame);

private:
  const TechProperties &properties;

  int m_samples_per_line;
  int samplesSync;
  int samplesBlank;
  int samplesBack;
  int samplesActive;
  int samplesBlackLeft;
  int samplesBlackRight;

  int samplesVSyncShort;
  int samplesVSyncLong;

  char levelSync;
  char levelBlank;
  char levelBlack;
  char levelWhite;
  char grayValues;

  int targetXres;
  int targetYres;
  int targetYresEven;
  int targetYresOdd;

  int linesEven;
  int linesOdd;
  int linesEvenActive;
  int linesOddActive;
  int linesEvenVisible;
  int linesOddVisible;
  int linesEvenBlankTop;
  int linesEvenBlankBottom;
  int linesOddBlankTop;
  int linesOddBlankBottom;

  float pixelAspect;

  uint16_t *line;
  uint16_t *m_end;
  uint16_t *m_ptr;

  void sendLine();

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

