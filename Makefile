#Para escrever comentários ##
############################# Makefile ##########################

COMPILAR = avr-gcc -g -Os -mmcu=atmega328p -c 


all: clean compilar comp hex gravar

compilar: clean HCSR serial controle comp hex

HCSR: 
	$(COMPILAR) HCSR.c

controle:
	$(COMPILAR) controle.c

serial:
	$(COMPILAR) serial.c

comp:
	$(COMPILAR) main.c && avr-gcc -g -mmcu=atmega328p -o main.elf serial.o main.o HCSR.o controle.o

hex:
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex

gravar:
	avrdude -p m328p -c arduino -P /dev/ttyUSB0 -b 57600 -U flash:w:main.hex

cleanpart:
	rm -rf *.elf *.hex

clean:
	rm -rf *.o *.elf *.hex