ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lDCECommon -lSerializeClass -lmysqlclient_r -lPlutoUtils

all: Main.o UpdateEntArea.o
	g++ Main.o UpdateEntArea.o -o UpdateEntArea $(LDFLAGS) $(LDLIBS)
	cp UpdateEntArea $(ROOT)/bin/

clean:
	rm -f UpdateEntArea UpdateEntArea.o
