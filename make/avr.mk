src=greenade
avrType=attiny24
avrFreq=8000000 # 8MHz for accurate baudrate timing
programmerType=usbasp
efuse=0xff
hfuse=0xdd
lfuse=0xe2
# todo - specify bitrate if required

cflags=-g -DF_CPU=$(avrFreq) -Wall -Os -Werror -Wextra

memoryTypes=calibration eeprom efuse flash fuse hfuse lfuse lock signature application apptable boot prodsig usersig

flash: elf
	avrdude -p$(avrType) -c$(programmerType) -U flash:w:$(src).elf

elf:
	avr-gcc $(cflags) -mmcu=$(avrType) -Wa,-ahlmnsc=$(src).lst -o $(src).elf $(src).S
	avr-objdump -C -d $(src).elf > $(src).objdump
	chmod a-x $(src).elf 2>&1

eeprom: elf
	avrdude -p$(avrType) -c$(programmerType) -U eeprom:w:$(src).elf

fuses:
	avrdude -B 100kHz -p$(avrType) -c$(programmerType) -U lfuse:w:$(lfuse):m
	avrdude -B 100kHz -p$(avrType) -c$(programmerType) -U hfuse:w:$(hfuse):m
	avrdude -B 100kHz -p$(avrType) -c$(programmerType) -U efuse:w:$(efuse):m

init: flash fuses

clean:
	rm $(src).elf $(src).lst $(src).objdump
	date

backup:
	@for memory in $(memoryTypes); do \
		avrdude -p $(avrType) -c$(programmerType) -U $$memory:r:./$(avrType).$$memory.hex:i; \
	done
