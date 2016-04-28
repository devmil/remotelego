PROJECT=legocontrol_atmega
MCU=atmega32
MCU_AVRDUDE=m32
OBJECTS = main.o uart.o


#-------------------------------------------------------------------
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature
COMMON = -mmcu=$(MCU)
CPU_FREQUENCY = 14745000UL
CFLAGS += $(COMMON) -I$(AVRLIB) -I. -std=gnu99 -Wall -Wextra -DF_CPU=$(CPU_FREQUENCY) -DUART_RX_BUFFER_SIZE=128 -DUART_TX_BUFFER_SIZE=128 -gdwarf-2 -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -Wno-deprecated-declarations -DUART_RX_BUFFER_SIZE=64 -DUART_TX_BUFFER_SIZE=64
LDFLAGS += $(COMMON) -Wl,--gc-sections




all: $(PROJECT).hex

%.o: %.c
	 avr-gcc $(INCLUDES) $(CFLAGS) $(CONLYFLAGS) -c  $<

$(PROJECT).elf: $(OBJECTS)
	 avr-gcc $(LDFLAGS) $(OBJECTS) -o $(PROJECT).elf
	
$(PROJECT).hex: $(PROJECT).elf
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS) $(PROJECT).elf $(PROJECT).hex
	@avr-size -C --mcu=${MCU} $(PROJECT).elf

flash: $(PROJECT).hex
	avrdude -p $(MCU_AVRDUDE) -c usbasp -B 0.2 -U flash:w:$(PROJECT).hex:a 

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
	-rm -rf $(OBJECTS) *.elf *.hex *.o
