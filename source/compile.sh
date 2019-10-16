echo
echo -------- begin --------
avr-gcc --version
#if exist "main.elf" echo Size before:  && avr-size --mcu=attiny85 --format=avr main.elf
#if not exist ".dep" mkdir .dep
echo
echo Compiling C: main.c
avr-gcc -c -mmcu=attiny85 -I. -gdwarf-2 -DF_CPU=1000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -Wa,-adhlns=./main.lst -I../ssd1306xled -std=gnu99 -MMD -MP -MF .dep/main.o.d main.c -o main.o
echo
echo Compiling C: ../ssd1306xled/ssd1306xled/ssd1306xled.c
avr-gcc -c -mmcu=attiny85 -I. -gdwarf-2 -DF_CPU=1000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -Wa,-adhlns=./../ssd1306xled/ssd1306xled/ssd1306xled.lst -I../ssd1306xled -std=gnu99 -MMD -MP -MF .dep/ssd1306xled.o.d ../ssd1306xled/ssd1306xled/ssd1306xled.c -o ../ssd1306xled/ssd1306xled/ssd1306xled.o
echo
echo Compiling C: ../ssd1306xled/ssd1306xled/ssd1306xledtx.c
avr-gcc -c -mmcu=attiny85 -I. -gdwarf-2 -DF_CPU=1000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -Wa,-adhlns=./../ssd1306xled/ssd1306xled/ssd1306xledtx.lst -I../ssd1306xled -std=gnu99 -MMD -MP -MF .dep/ssd1306xledtx.o.d ../ssd1306xled/ssd1306xled/ssd1306xledtx.c -o ../ssd1306xled/ssd1306xled/ssd1306xledtx.o
echo
echo Compiling C: ../ssd1306xled/tinyavrlib/num2str.c
avr-gcc -c -mmcu=attiny85 -I. -gdwarf-2 -DF_CPU=1000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -Wa,-adhlns=./../ssd1306xled/tinyavrlib/num2str.lst -I../ssd1306xled -std=gnu99 -MMD -MP -MF .dep/num2str.o.d ../ssd1306xled/tinyavrlib/num2str.c -o ../ssd1306xled/tinyavrlib/num2str.o
echo
echo Linking: main.elf
avr-gcc -mmcu=attiny85 -I. -gdwarf-2 -DF_CPU=1000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -Wa,-adhlns=main.o -I../ssd1306xled -std=gnu99 -MMD -MP -MF .dep/main.elf.d main.o ../ssd1306xled/ssd1306xled/ssd1306xled.o ../ssd1306xled/ssd1306xled/ssd1306xledtx.o ../ssd1306xled/tinyavrlib/num2str.o --output main.elf -Wl,-Map=main.map,--cref     -lm
echo
echo Creating load file for Flash: main.hex
avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock main.elf main.hex
echo
echo Creating load file for EEPROM: main.eep
avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 --no-change-warnings -O ihex main.elf main.eep || exit 0
echo
echo Creating Extended Listing: main.lss
avr-objdump -h -S -z main.elf > main.lss
echo
echo Creating Symbol Table: main.sym
avr-nm -n main.elf > main.sym
#if exist "main.elf" echo Size after: && avr-size --mcu=attiny85 --format=avr main.elf
echo --------  end  --------
echo

