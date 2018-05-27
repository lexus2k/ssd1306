@rem    MIT License
@rem
@rem    Copyright (c) 2018, Alexey Dynda
@rem
@rem    Permission is hereby granted, free of charge, to any person obtaining a copy
@rem    of this software and associated documentation files (the "Software"), to deal
@rem    in the Software without restriction, including without limitation the rights
@rem    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
@rem    copies of the Software, and to permit persons to whom the Software is
@rem    furnished to do so, subject to the following conditions:
@rem
@rem    The above copyright notice and this permission notice shall be included in all
@rem    copies or substantial portions of the Software.
@rem
@rem    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
@rem    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
@rem    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
@rem    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
@rem    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
@rem    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
@rem    SOFTWARE.
@rem

@if "%1" == "" goto error

set project=%1
@rem remove quotes and replace backslash
@set sketch_path=%project:"=%
@set sketch_path=%sketch_path:/=\%

@rem mkdir ..\bld\
@mkdir ..\bld\%sketch_path%
@copy /Y sdl\SDL2.dll ..\bld\%sketch_path%\..\

mingw32-make.exe -C ../examples -f Makefile.mingw32 EXTRA_CCFLAGS=%2 SDL_EMULATION=y PROJECT=%project% flash %3
@exit /0

:error
@echo "Usage: build_and_run.bat <project_name> ["flags"] [option]"
@echo "       flags:"
@echo "           -DARKANOID_SSD1331   to run in SSD1331 RGB color mode"
@echo "           -DSDL_NO_BORDER      to not draw border around oled display emulator"
@echo "       option:"
@echo "           "ADAFRUIT=y"         to enable Adafruit support"
@echo -
@echo "Example: build_and_run.bat "games/arkanoid" "-DARKANOID_SSD1331 -DSDL_NO_BORDER""