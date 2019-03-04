/*
 * Credits to https://github.com/bitluni/ESP32CompositeVideo
 */

#include "CompositeOutput.h"

#if defined(CONFIG_VGA_AVAILABLE) && defined(CONFIG_VGA_ENABLE) && defined(__XTENSA__)

#include "driver/i2s.h"

#define I2S_VGA_SAMPLE_RATE   (4000000)
static const i2s_port_t I2S_PORT = (i2s_port_t)I2S_NUM_0;

const TechProperties PALProperties = {
  .lineMicros = 64,
  .syncMicros = 4.7,
  .blankEndMicros = 10.4,
  .backMicros = 1.65,
  .shortVSyncMicros = 2.35,
  .overscanLeftMicros = 1.6875,
  .overscanRightMicros = 1.6875,
  .syncVolts = -0.3,
  .blankVolts = 0.0,
  .blackVolts =  0.005,//specs 0.0,
  .whiteVolts = 0.7,
  .lines = 625,
  .linesFirstTop = 23,
  .linesOverscanTop = 9,
  .linesOverscanBottom = 9,
  .imageAspect = 4./3.
};

const TechProperties NTSCProperties = {
  .lineMicros = 63.492,
  .syncMicros = 4.7,
  .blankEndMicros = 9.2,
  .backMicros = 1.5,
  .shortVSyncMicros = 2.3,
  .overscanLeftMicros = 0,//1.3,
  .overscanRightMicros = 0,//1,
  .syncVolts = -0.286,
  .blankVolts = 0.0,
  .blackVolts = 0.05, //specs 0.054,
  .whiteVolts = 0.714,
  .lines = 525,
  .linesFirstTop = 20,
  .linesOverscanTop = 6,
  .linesOverscanBottom = 9,
  .imageAspect = 4./3.
};

CompositeOutput::CompositeOutput(Mode mode, double Vcc)
    :properties((mode==NTSC) ? NTSCProperties: PALProperties)
{
    double dacPerVolt = 255.0 / Vcc;
    levelSync = 0;
    levelBlank = (properties.blankVolts - properties.syncVolts) * dacPerVolt + 0.5;
    levelBlack = (properties.blackVolts - properties.syncVolts) * dacPerVolt + 0.5;
    levelWhite = (properties.whiteVolts - properties.syncVolts) * dacPerVolt + 0.5;
    grayValues = levelWhite - levelBlack + 1;
}

void CompositeOutput::init(int xres, int yres, int bpp)
{
    const int LINES_SYNC_TOP = 5;
    const int LINES_SYNC_BOTTOM = 3;

    m_buffer_width = xres;
    m_buffer_height = yres;
    m_bpp = bpp;

    linesOdd = properties.lines / 2;
    linesEven = properties.lines - linesOdd;
    linesEvenActive = linesEven - properties.linesFirstTop - LINES_SYNC_BOTTOM;
    linesOddActive = linesOdd - properties.linesFirstTop - LINES_SYNC_BOTTOM;
    linesEvenVisible = linesEvenActive - properties.linesOverscanTop - properties.linesOverscanBottom;
    linesOddVisible = linesOddActive - properties.linesOverscanTop - properties.linesOverscanBottom;

    targetYresOdd = (yres / 2 < linesOddVisible) ? yres / 2 : linesOddVisible;
    targetYresEven = (yres - targetYresOdd < linesEvenVisible) ? yres - targetYresOdd : linesEvenVisible;
    targetYres = targetYresEven + targetYresOdd;

    linesEvenBlankTop = properties.linesFirstTop - LINES_SYNC_TOP + properties.linesOverscanTop + (linesEvenVisible - targetYresEven) / 2;
    linesEvenBlankBottom = linesEven - linesEvenBlankTop - targetYresEven - LINES_SYNC_BOTTOM;
    linesOddBlankTop = linesEvenBlankTop;
    linesOddBlankBottom = linesOdd - linesOddBlankTop - targetYresOdd - LINES_SYNC_BOTTOM;

    double samplesPerSecond = I2S_VGA_SAMPLE_RATE;
    double samplesPerMicro = samplesPerSecond * 0.000001;
    m_samples_per_line = (int)(samplesPerMicro * properties.lineMicros + 1.5) & ~1;
    samplesSync = samplesPerMicro * properties.syncMicros + 0.5;
    samplesBlank = samplesPerMicro * (properties.blankEndMicros - properties.syncMicros + properties.overscanLeftMicros) + 0.5;
    samplesBack = samplesPerMicro * (properties.backMicros + properties.overscanRightMicros) + 0.5;
    samplesActive = m_samples_per_line - samplesSync - samplesBlank - samplesBack;

    targetXres = xres < samplesActive ? xres : samplesActive;

    samplesVSyncShort = samplesPerMicro * properties.shortVSyncMicros + 0.5;

    samplesBlackLeft = (samplesActive - targetXres) / 2;
    samplesBlackRight = samplesActive - targetXres - samplesBlackLeft;

//    pixelAspect = (float(samplesActive) / (linesEvenVisible + linesOddVisible)) / properties.imageAspect;

    line = (uint16_t*)malloc(sizeof(uint16_t) * m_samples_per_line * 2);
    m_ptr = line;
    m_end = line + m_samples_per_line * 2;

    init_hardware();
}

void CompositeOutput::init_hardware()
{
    i2s_config_t i2s_config = {
       .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
       .sample_rate = I2S_VGA_SAMPLE_RATE,  //not really used
       .bits_per_sample = (i2s_bits_per_sample_t)I2S_BITS_PER_SAMPLE_16BIT,
       .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
       .communication_format = I2S_COMM_FORMAT_I2S_MSB,
       .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
       .dma_buf_count = 2,
       .dma_buf_len = m_samples_per_line  //a buffer per line
    };

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);    //start i2s driver
    i2s_set_pin(I2S_PORT, NULL);                           //use internal DAC
    i2s_set_sample_rates(I2S_PORT, I2S_VGA_SAMPLE_RATE);   //dummy sample rate, since the function fails at high values
}

void CompositeOutput::fillValues(uint8_t value, int count)
{
    for(int j = 0; j < count; j++)
    {
        *m_ptr = ((uint16_t)value << 8);
        m_ptr++;
    }
}

void CompositeOutput::sendFrameHalfResolution(const uint8_t *frame)
{
    generate_long_sync();       // 1
    generate_long_sync();       // 2
    generate_long_short_sync(); // 3
    generate_short_sync();      // 4
    generate_short_sync();      // 5
    for (int y = 0; y < linesEvenBlankTop; y++)
    {
        generate_blank_line();  // top blank lines
    }

    for (int y = 0; y < targetYresEven; y++)
    {
        generate_line_from_buffer( &frame[(y >> 3)*(m_buffer_width * m_bpp / 8)] );  // real data
    }
    for (int y = 0; y < linesEvenBlankBottom; y++)
    {
        generate_blank_line();  // bottom blank lines
    }
    generate_short_sync();      // 311
    generate_short_sync();      // 312

    generate_short_long_sync();      // 313
    generate_long_sync();       // 314
    generate_long_sync();       // 315
    generate_short_sync();      // 316
    generate_short_sync();      // 317

    generate_short_blank_sync(); // 318
    for (int y = 0; y < linesOddBlankTop; y++)
    {
        generate_blank_line();  // top blank lines
    }
    for (int y = 0; y < targetYresOdd; y++)
    {
        generate_line_from_buffer( &frame[(y>>3)*(m_buffer_width * m_bpp / 8)] );  // real data
    }
    for(int y = 0; y < linesOddBlankBottom; y++)
    {
        generate_blank_line();  // bottom blank lines
    }
    generate_blank_short_sync();  // 623
    generate_short_sync();        // 624
    generate_short_sync();        // 625
    if (m_ptr != line) // force to send data
    {
        i2s_write_bytes(I2S_PORT, (char*)line, (m_ptr - line) * sizeof(uint16_t), portMAX_DELAY);
        m_ptr = line;
    }
}

void CompositeOutput::generate_long_sync()
{
    for (int i = 0; i < 2; i++)
    {
        fillValues(levelSync, m_samples_per_line / 2 - samplesVSyncShort);
        fillValues(levelBlank, samplesVSyncShort);
    }
    check_buffer();
}

void CompositeOutput::generate_short_sync()
{
    for (int i = 0; i < 2; i++)
    {
        fillValues(levelSync, samplesVSyncShort);
        fillValues(levelBlank, m_samples_per_line / 2 - samplesVSyncShort);
    }
    check_buffer();
}

void CompositeOutput::generate_long_short_sync()
{
    fillValues(levelSync, m_samples_per_line / 2 - samplesVSyncShort);
    fillValues(levelBlank, samplesVSyncShort);
    fillValues(levelSync, samplesVSyncShort);
    fillValues(levelBlank, m_samples_per_line / 2 - samplesVSyncShort);
    check_buffer();
}

void CompositeOutput::generate_short_long_sync()
{
    fillValues(levelSync, samplesVSyncShort);
    fillValues(levelBlank, m_samples_per_line / 2 - samplesVSyncShort);
    fillValues(levelSync, m_samples_per_line / 2 - samplesVSyncShort);
    fillValues(levelBlank, samplesVSyncShort);
    check_buffer();
}

void CompositeOutput::generate_short_blank_sync()
{
    fillValues(levelSync, samplesVSyncShort);
    fillValues(levelBlank, m_samples_per_line / 2 - samplesVSyncShort);
    fillValues(levelBlank, m_samples_per_line / 2);
    check_buffer();
}

void CompositeOutput::generate_blank_short_sync()
{
    fillValues(levelSync, samplesSync);
    fillValues(levelBlank, m_samples_per_line / 2 - samplesSync);
    fillValues(levelSync, samplesVSyncShort);
    fillValues(levelBlank, m_samples_per_line / 2 - samplesVSyncShort);
    check_buffer();
}

void CompositeOutput::generate_blank_line()
{
    fillValues(levelSync, samplesSync);
    fillValues(levelBlank, samplesBlank);
    fillValues(levelBlack, samplesActive);
    fillValues(levelBlank, samplesBack);
    check_buffer();
}

const uint8_t* CompositeOutput::generate_line_from_buffer(const uint8_t *pixels)
{
    fillValues(levelSync, samplesSync);
    fillValues(levelBlank, samplesBlank);
    fillValues(levelBlack, samplesBlackLeft);
    for (int x = 0; x < targetXres; x++)
    {
        uint8_t color = pixels[(x >> 3)] & (1<< (x&0x07));
//        *m_ptr = (levelBlack + pixels[x]) << 8;
        *m_ptr = (levelBlack + (color ? 0x80: 0x00 )) << 8;
        m_ptr++;
    }
    fillValues(levelBlack, samplesBlackRight);
    fillValues(levelBlank, samplesBack);
    check_buffer();
    return pixels + m_buffer_width * m_bpp / 8;
//    return pixels + targetXres;
}

void CompositeOutput::check_buffer()
{
    if (m_ptr == m_end)
    {
        i2s_write_bytes(I2S_PORT, (char*)line, sizeof(uint16_t) * (m_end - line), portMAX_DELAY);
        m_ptr = line;
    }
}

#endif

