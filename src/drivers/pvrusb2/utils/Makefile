
# $Id: Makefile 1352 2006-09-29 06:02:21Z isely $

all: decode_log decode_eeprom 

clean:
	rm -f decode_log decode_eeprom regdump

decode_log: decode_log.cpp
	g++ -g -o decode_log decode_log.cpp

decode_eeprom: decode_eeprom.cpp
	g++ -g -o decode_eeprom decode_eeprom.cpp

regdump: regdump.c
	gcc -o regdump -I$(KDIR)/include regdump.c
