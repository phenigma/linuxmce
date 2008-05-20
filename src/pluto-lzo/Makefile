ROOT=..

CPPFLAGS=-I. -I..
CXXFLAGS=-W -Wall -g -ggdb3
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lPlutoUtils -lDCECommon -lpthread -lSerializeClass

ifeq ($(ARCH),mipsel)
	CXX=mipsel-linux-g++
else
	CXX=g++
endif

all: pluto-lzo

clean:
	rm -f pluto-lzo
