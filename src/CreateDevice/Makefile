ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lDCECommon -lSerializeClass -lmysqlclient_r -lPlutoUtils

all: Main.o CreateDevice.o
	g++ Main.o CreateDevice.o -o CreateDevice $(LDFLAGS) $(LDLIBS)
	cp CreateDevice $(ROOT)/bin/

clean:
	rm -f CreateDevice CreateDevice.o
