ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lpluto_main -lSerializeClass -lDCECommon -lDCECommon -lmysqlclient_r -lPlutoUtils

all: MakeRelease.o MakeRelease

clean:
	rm -f MakeRelease MakeRelease.o
