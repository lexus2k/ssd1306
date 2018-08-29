# -*- coding: UTF-8 -*-
#    MIT License
#
#    Copyright (c) 2018, Alexey Dynda
#
#    Permission is hereby granted, free of charge, to any person obtaining a copy
#    of this software and associated documentation files (the "Software"), to deal
#    in the Software without restriction, including without limitation the rights
#    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#    copies of the Software, and to permit persons to whom the Software is
#    furnished to do so, subject to the following conditions:
#
#    The above copyright notice and this permission notice shall be included in all
#    copies or substantial portions of the Software.
#
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#    SOFTWARE.
#
###################################################################################
# Fixing pip
# curl https://bootstrap.pypa.io/get-pip.py | python2
# sudo pip install freetype-py
#
# Get fonts
# wget https://ftp.gnu.org/gnu/freefont/freefont-ttf-20120503.zip

import sys
import freetype
import modules.ttfsource
import modules.fontgenerator

def print_help_and_exit():
    print "Usage: ttf_fonts.py ttfFile [args] > outputFile"
    print "args:"
    print "      -s N    font size (this is not pixels!) "
    print "      -SB N   limit size in pixels to value (pixels will be cut)"
    print "      -fh     fixed height"
    print "      -fw     fixed width"
    print "      -fo     format old 1.7.6 and below"
    print "      -d      Print demo text to console"
    print "Examples:"
    print "   [generate font in old library format to file]"
    print "      ttf_fonts.py FreeSans -s 8 -fo > ssd1306font.c"
    print "   [demo text for generated font]"
    print "      ttf_fonts.py FreeSans -d"
    print "Font name must not include .ttf extenstion. Tool will add it"
    exit(1)

if len(sys.argv) < 2:
    print_help_and_exit()

fsize = 8
fold = False
flimit_bottom = 0
fwidth = False
fheight = False
fname = sys.argv[1]
demo_text = False

# parse args
idx = 2
while idx < len(sys.argv):
    opt = sys.argv[idx]
    if opt == "-s":
        idx += 1
        fsize = int(sys.argv[idx])
    elif opt == "-SB":
        idx +=1
        flimit_bottom = int(sys.argv[idx])
    elif opt == "-fh":
        fheight = True
    elif opt == "-fw":
        fwidth = True
    elif opt == "-fo":
        fold = True
    elif opt == "-d":
        demo_text = True
    else:
        print "Unknown option: ", opt
        print_help_and_exit()
    idx += 1

source = modules.ttfsource.FontSource(fname, fsize)
source.add_chars(' ', unichr(127))
if fwidth:
    source.expand_chars_h()
if fheight:
    source.expand_chars_v()
if flimit_bottom > 0:
    source.deflate_chars_bottom( flimit_bottom )
#source.printChar('q')
#source.printString('World!q01')
#source.printChar('Q')
#source.printChar('q')
#source.printChar('Q')

font = modules.fontgenerator.Generator( source )
if fold:
    source.expand_chars()
    if demo_text:
        source.printString('World!q01')
    font.generate_fixed_old()
else:
    if demo_text:
        source.printString('World!q01')
    font.generate_new_format()

