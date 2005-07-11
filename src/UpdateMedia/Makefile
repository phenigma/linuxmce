ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib
LDLIBS= -lpluto_media -lpluto_main -lDCECommon -lSerializeClass -lmysqlclient_r -lPlutoUtils -lpthread -lattr

all: Main.o UpdateMedia.o
	g++ Main.o UpdateMedia.o -o UpdateMedia $(LDFLAGS) $(LDLIBS)
	cp UpdateMedia $(ROOT)/bin/

clean:
	rm -f UpdateMedia UpdateMedia.o
