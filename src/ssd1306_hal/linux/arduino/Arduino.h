#ifndef _ARDUINO_H_
#define _ARDUINO_H_

#define ARDUINO_FAKE
#if defined(__MINGW32__)
#include "ssd1306_hal/mingw/io.h"
#include "ssd1306_hal/Print_internal.h"
#else
#include "ssd1306_hal/linux/io.h"
#include "ssd1306_hal/Print_internal.h"
#endif

#ifndef boolean
    typedef uint8_t boolean;
#endif

#endif

