ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lDCECommon -lSerializeClass -lmysqlclient_r -lPlutoUtils

all: Main.o TestSerialPort.o UserUtils.o
	g++ Main.o TestSerialPort.o UserUtils.o -o TestSerialPort $(LDFLAGS) $(LDLIBS)
	cp TestSerialPort $(ROOT)/bin/

clean:
	rm -f TestSerialPort TestSerialPort.o
