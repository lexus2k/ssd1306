#!/bin/sh

if [ "$1" = "" ]; then
    echo "Usage: ./build_and_flash.sh example"
    exit 1
fi

make -C ../examples -f Makefile.avr PROJECT=$1 flash