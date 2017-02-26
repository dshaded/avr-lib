lfuse      ?= !!!changeme!!!
hfuse      ?= !!!changeme!!!
efuse      ?= !!!changeme!!!
clock      ?= 8000000
mmcu       ?= atmega8
fuseClock  ?= 100kHz
srcDirs    ?= src lib/src
cflags     ?= -Os -Wall -Werror -Wextra
#prog      := default programmer from ~/.avrduderc
#port      := not specified
#progClock := programmer default

cflagsReal := $(cflags) -g -DF_CPU=$(clock) -mmcu=$(mmcu) -iquotedir lib/src
dudeFlags := -p $(mmcu)
ifdef prog
  dudeFlags += -c $(prog)
endif
ifdef port
  dudeFlags += -P $(port)
endif
fuseFlags := $(dudeFlags) -B $(fuseClock)
ifdef progClock
  dudeFlags += -B $(progClock)
endif

HIDE := @
TRASH := @
ifeq ($(VERBOSE),1)
  HIDE :=
endif
ifeq ($(VERBOSE),2)
  HIDE :=
  TRASH :=
endif

.PHONY: all clean install fuse flash eeprom lfuse hfuse efuse mkdirs

all: out/objdump.txt out/flash.hex out/eeprom.hex

clean:
	$(TRASH)rm -rf out build

install: all fuse flash eeprom

fuse: lfuse hfuse efuse

flash eeprom: % :out/%.hex
	avrdude $(dudeFlags) -U $@:w:$<

lfuse hfuse efuse:
	avrdude $(fuseFlags) -U $@:w:$($@):m


#################### INTERNAL TARGETS ####################
sourceDirs != find $(srcDirs) -type d
sources != find $(srcDirs) -type f -and \( -name '*.[csS]' -or -name '*.sx' \)
objects := $(addsuffix .o,$(addprefix build/,$(sources)))
makeDeps := $(addsuffix .mk,$(addprefix build/,$(sources)))

mkdirs:
	$(TRASH)for dir in $(sourceDirs); do mkdir -p build/$$dir; done
	$(TRASH)mkdir -p out/

$(objects):
	$(HIDE)avr-gcc $(cflagsReal) -Wa,-ahlmnsc=$(@:.o=.lst) -c -o $@ $<

$(makeDeps): build/%.mk : % | mkdirs
	$(HIDE)avr-gcc $(cflagsReal) -MM -MT build/$<.o -o $@ $<

build/a.out: $(objects)
	$(HIDE)avr-gcc $(cflagsReal) -o $@ $^

out/objdump.txt: build/a.out
	$(TRASH)avr-objdump -C -d $< > $@

out/flash.hex: build/a.out
	$(TRASH)avr-objcopy -j .text -O ihex $< $@

out/eeprom.hex: build/a.out
	$(TRASH)avr-objcopy -j .eeprom -O ihex $< $@



include $(makeDeps)
