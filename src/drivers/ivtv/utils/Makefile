PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
HDRDIR = /usr/include/linux

EXES := ivtvctl ivtvfbctl ivtvplay cx25840ctl ivtv-detect \
        ivtv-radio ivtv-tune/ivtv-tune ivtv-mpegindex ivtv-encoder 

HEADERS := ../driver/ivtv.h

CFLAGS = -I../driver -D_GNU_SOURCE -O2 -Wall
CXXFLAGS = $(CFLAGS)
export CFLAGS CXXFLAGS

all: $(EXES)

ivtvctl: ivtvctl.o
	$(CC) -lm -o $@ $^

ivtvctl.o: ../driver/ivtv-svnversion.h

ivtvplay: ivtvplay.cc
	$(CXX) $(CXXFLAGS) -lm -lpthread -o $@ $^

ivtv-tune/ivtv-tune:
	$(MAKE) -C ivtv-tune

encoder.o: encoder.c
	$(CC) $(CFLAGS) -DVIDEO_PORT=0 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -c $^

ivtv-encoder: enc_mindex.o enc_chann.o encoder.o
	$(CC) -lpthread -o $@ $^

cx25840-registers.o: ../driver/cx25840-registers.c
	$(CC) $(CFLAGS) -DCX25840_STRINGS -c $^

cx25840ctl: cx25840ctl.o cx25840-registers.o

install: $(EXES)
	install -d $(DESTDIR)/$(HDRDIR)
	install -m 0644 $(HEADERS) $(DESTDIR)/$(HDRDIR)
	install -d $(DESTDIR)/$(BINDIR)
	install -m 0755 $(EXES) $(DESTDIR)/$(BINDIR)

clean: 
	rm -f *.o $(EXES)
	$(MAKE) -C ivtv-tune clean
	
../driver/ivtv-svnversion.h:
	$(MAKE) -C ../driver ivtv-svnversion.h

.PHONY: ../driver/ivtv-svnversion.h
