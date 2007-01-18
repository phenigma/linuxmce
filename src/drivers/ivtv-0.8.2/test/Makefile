INSTALLDIR = /usr/local/bin
HEADERDIR = /usr/include/linux
KVER ?= $(shell uname -r)
INCDIR = /lib/modules/$(KVER)/build/include

EXES = vbi wss vbi-passthrough vbi-detect ivtv-pcm-tester

CFLAGS = -I../driver -I../utils -I$(INCDIR) -D_GNU_SOURCE -O2 -Wall
CXXFLAGS = $(CFLAGS)
LDFLAGS = -lm

all: $(EXES)

install: all

clean: 
	rm -f *.o $(EXES)
