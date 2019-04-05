clock      ?= 8000000
mmcu       ?= atmega8
mmcudef    ?= $(mmcu)
fuseClock  ?= 100kHz
srcDirs    ?= src lib/src
cflags     ?= -Os -Wall -Werror -Wextra
#lfuse     := skipped
#hfuse     := skipped
#efuse     := skipped
#prog      := default programmer from ~/.avrduderc
#port      := not specified
#progClock := programmer default

cflagsReal := $(cflags) -ggdb -DF_CPU=$(clock) -mmcu=$(mmcudef) -iquotelib/src
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
ifeq ($(verbose),1)
  HIDE :=
endif
ifeq ($(verbose),2)
  HIDE :=
  TRASH :=
endif

knownFuses := lfuse hfuse efuse
usedFuses := $(filter $(knownFuses),$(.VARIABLES))

.PHONY: all clean install fuse flash eeprom mkdirs $(knownFuses)

all: out/objdump.txt out/flash.hex out/eeprom.hex stats

stats: build/a.out
	avr-size -C --mcu=$(mmcu) build/a.out

clean:
	$(TRASH)rm -rf out build

install: all fuse flash eeprom

fuse: $(usedFuses)

flash eeprom: % :out/%.hex out/objdump.txt
	@echo AvrDude writing $@ from $<
	$(HIDE)avrdude $(dudeFlags) -U $@:w:$<

$(usedFuses):
	@echo AvrDude setting $@ to $($@)
	$(HIDE)avrdude $(fuseFlags) -U $@:w:$($@):m


#################### INTERNAL TARGETS ####################
sourceDirs != find -L $(srcDirs) -type d
sources != find -L $(srcDirs) -type f -and \( -name '*.[csS]' -or -name '*.sx' -or -name '*.cpp' \)
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
	$(TRASH)avr-objdump -C -S $< > $@

out/flash.hex: build/a.out
	$(TRASH)avr-objcopy -j .text -j .data -O ihex $< $@

out/eeprom.hex: build/a.out
	$(TRASH)avr-objcopy -j .eeprom -O ihex $< $@



include $(makeDeps)
