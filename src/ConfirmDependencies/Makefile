ROOT=..

CXX=g++
CPPFLAGS=
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -ggdb3
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lpluto_main -lPlutoUtils -lSerializeClass -lDCECommon -lmysqlclient_r -lDCECommon

all: ConfirmDependencies
	cp ConfirmDependencies ../bin

clean:
	rm -f ConfirmDependencies
