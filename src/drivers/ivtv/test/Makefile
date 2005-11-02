INSTALLDIR = /usr/local/bin
HEADERDIR = /usr/include/linux

EXES = vbi fwapi wss vbi-passthrough vbi-detect

LIB_ZVBI_SRC = /home/src/zvbi/zvbi-0.2.15/src/.libs
INC_ZVBI_SRC = /home/src/zvbi/zvbi-0.2.15/src

CFLAGS = -I../driver -I../utils -D_GNU_SOURCE -O2 -Wall
CXXFLAGS = $(CFLAGS)
LDFLAGS = -lm

all: $(EXES)

capture: cc_decode.h capture.c
	gcc $(CFLAGS) -I$(INC_ZVBI_SRC) -o capture capture.c  $(LIB_ZVBI_SRC)/libzvbi.a -lpthread -lm -lz 

install: all

clean: 
	rm -f *.o $(EXES) capture
