ROOT=..

CXX=g++
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lpluto_main -lSerializeClass -lPlutoUtils -lDCECommon -lmysqlclient_r

all: MakeRelease

clean:
	rm -f MakeRelease
