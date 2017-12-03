"C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avr-nm.exe" --size-sort -C -r %1 >elfdump.txt
"C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avr-objdump.exe" -C -d %1 >elfdisasm.txt
