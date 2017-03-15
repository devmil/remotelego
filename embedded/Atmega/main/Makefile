PROJECT=legocontrol_atmega
MCU=atmega32
MCU_AVRDUDE=m32
BUILDDIR = build
SOURCE_FILES = $(wildcard *.c)
OBJECTS = $(addprefix $(BUILDDIR)/, $(SOURCE_FILES:.c=.o))

#-------------------------------------------------------------------
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature
COMMON = -mmcu=$(MCU)
CPU_FREQUENCY = 14745000UL
CFLAGS += $(COMMON) -I$(AVRLIB) -I. -std=gnu99 -Wall -Wextra -DF_CPU=$(CPU_FREQUENCY) -DUART_RX_BUFFER_SIZE=128 -DUART_TX_BUFFER_SIZE=128 -gdwarf-2 -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -Wno-deprecated-declarations -DUART_RX_BUFFER_SIZE=64 -DUART_TX_BUFFER_SIZE=64
LDFLAGS += $(COMMON) -Wl,--gc-sections


.PHONY: rebuild
rebuild: clean all

.PHONY: all
all: $(BUILDDIR)/$(PROJECT).hex

$(BUILDDIR):
	mkdir $(BUILDDIR)

$(BUILDDIR)/%.o: %.c $(BUILDDIR)
	 avr-gcc $(INCLUDES) $(CFLAGS) $(CONLYFLAGS) -c -o $@ $<

$(BUILDDIR)/$(PROJECT).elf: $(OBJECTS)
	 avr-gcc $(LDFLAGS) $(OBJECTS) -o $(BUILDDIR)/$(PROJECT).elf
	
$(BUILDDIR)/$(PROJECT).hex: $(BUILDDIR)/$(PROJECT).elf
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS) $(BUILDDIR)/$(PROJECT).elf $(BUILDDIR)/$(PROJECT).hex
	@avr-size -C --mcu=${MCU} $(BUILDDIR)/$(PROJECT).elf

flash: $(BUILDDIR)/$(PROJECT).hex
	avrdude -p $(MCU_AVRDUDE) -c usbasp -B 0.2 -U flash:w:$(BUILDDIR)/$(PROJECT).hex:a 

#sets the AVR to use a fast external oscillator
#http://www.engbedded.com/fusecalc/

#Default
#+ Ext. Crystal/Resonator High Freq.; Start-up time: 16K CK + 64 ms; [CKSEL=1111 SUT=11]
#+ Brown out detection
#+ CKOPT
.PHONY: prepare_avr
prepare_avr:
	avrdude -p $(MCU_AVRDUDE) -c usbasp -B 3 -U lfuse:w:0xbf:m -U hfuse:w:0x89:m

.PHONY: clean
clean:
	-rm -rf $(BUILDDIR)
