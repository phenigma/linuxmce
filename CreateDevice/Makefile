ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lpluto_main -lSerializeClass -lDCECommon -lDCECommon -lmysqlclient_r -lPlutoUtils

all: CreateDevice.o CreateDevice

clean:
	rm -f CreateDevice CreateDevice.o
