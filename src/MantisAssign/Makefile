ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lDCECommon -lSerializeClass -lmysqlclient_r -lPlutoUtils

all: Main.o MantisAssign.o UserUtils.o
	g++ Main.o MantisAssign.o UserUtils.o -o MantisAssign $(LDFLAGS) $(LDLIBS)
	cp MantisAssign $(ROOT)/bin/

clean:
	rm -f MantisAssign MantisAssign.o
