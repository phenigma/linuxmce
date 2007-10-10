ROOT = ../..
CPPFLAGS=-g -W -Wall
CXXFLAGS=-fPIC
LDFLAGS=-L../libresolution
LDLIBS=-lresolution

sources=$(wildcard *.cpp)

all: X-GetModeline

X-GetModeline: X-GetModeline.o
	g++ $(CPPFLAGS) $(CXXFLAGS) $< -o X-GetModeline $(LDFLAGS) $(LDLIBS)
	cp X-GetModeline $(ROOT)/bin

clean:
	rm -f X-GetModeline *.o

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
