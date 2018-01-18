#!/bin/sh

if [ "$1" = "" ]; then
    echo "Usage: avrparse.sh file.out"
    exit 1
fi

avr-nm --size-sort -C -r $1 >elfdump.txt
avr-objdump -C -d $1 >elfdisasm.txt
