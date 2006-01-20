PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
HDRDIR = /usr/include/linux

EXES := ivtvctl ivtv-detect ivtv-radio 
EXES := $(shell if echo - | $(CC) -E -dM - | grep __powerpc__ > /dev/null; \
	then echo $(EXES); else \
	echo $(EXES) ivtvfbctl ivtvplay ivtv-mpegindex ivtv-encoder; fi)
BIN := $(EXES) ivtv-tune/ivtv-tune cx25840ctl/cx25840ctl


HEADERS := ../driver/ivtv.h

CFLAGS = -I$(CURDIR) -I$(CURDIR)/../driver -D_GNU_SOURCE -O2 -Wall
CXXFLAGS = $(CFLAGS)

all: $(EXES)
	$(MAKE) CFLAGS="$(CFLAGS)" -C ivtv-tune
	$(MAKE) CFLAGS="$(CFLAGS)" -C cx25840ctl

ivtvctl: ivtvctl.o
	$(CC) -lm -o $@ $^

ivtvctl.c: ../driver/ivtv-svnversion.h

ivtvplay: ivtvplay.cc
	$(CXX) $(CXXFLAGS) -lm -lpthread -o $@ $^

encoder.o: encoder.c
	$(CC) $(CFLAGS) -DVIDEO_PORT=0 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -c $^

ivtv-encoder: enc_mindex.o enc_chann.o encoder.o
	$(CC) -lpthread -o $@ $^

install: all
	install -d $(DESTDIR)/$(HDRDIR)
	install -m 0644 $(HEADERS) $(DESTDIR)/$(HDRDIR)
	install -d $(DESTDIR)/$(BINDIR)
	install -m 0755 $(BIN) $(DESTDIR)/$(BINDIR)

clean: 
	rm -f *.o $(EXES)
	$(MAKE) -C ivtv-tune clean
	$(MAKE) -C cx25840ctl clean
	
../driver/ivtv-svnversion.h:
	$(MAKE) -C ../driver ivtv-svnversion.h

.PHONY: ../driver/ivtv-svnversion.h
