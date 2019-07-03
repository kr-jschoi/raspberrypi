PORT=dev/ttyAMA0 # PORT[UART=ttyAMA0, SPI=spidev0.0]
MCU=atmega328p
CFLAGS=-g -Wall -mcall-prologues -mmcu=$(MCU) -Os # compilefile option
LDFLAGS=-Wl,-gc-sections -Wl,-relax #linkfile option
CC=avr-gcc
TARGET=main
OBJECT_FILES=main.o

run: clean all flash

clean:
	rm -f *.o *.hex *.obj *.hex
	rm -rf html latex

all: $(TARGET).hex

%.hex: %.obj
	avr-objcopy -R .eeprom -O ihex $< $@

%.obj: $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(OBJECT_FILES) $(LDFLAGS) -o $@

flash: $(TARGET).hex
	avrdude -p $(MCU) -c gpio -P $(PORT) -U flash:w:$(TARGET).hex

docs: clean
	doxygen doc
