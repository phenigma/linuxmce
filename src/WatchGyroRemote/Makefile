ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lDCECommon -lPlutoUtils -lpthread -lhid

all: Main.o 
	g++ Main.o -o WatchGyroRemote $(LDFLAGS) $(LDLIBS)
	cp WatchGyroRemote $(ROOT)/bin/

clean:
	rm -f WatchGyroRemote WatchGyroRemote.o
