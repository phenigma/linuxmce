INSTALLDIR = /usr/local/bin
HEADERDIR = /usr/include/linux

EXES = vbi fwapi wss vbi-passthrough vbi-detect ivtv-pcm-tester

CFLAGS = -I../driver -I../utils -D_GNU_SOURCE -O2 -Wall
CXXFLAGS = $(CFLAGS)
LDFLAGS = -lm

all: $(EXES)

install: all

clean: 
	rm -f *.o $(EXES)
