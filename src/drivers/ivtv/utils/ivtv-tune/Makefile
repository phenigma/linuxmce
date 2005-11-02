CFLAGS = -I.. -D_GNU_SOURCE -O2 -Wall
CXXFLAGS = $(CFLAGS)

all: cmdline.o ivtv-tune

clean:
	rm -f *.o ivtv-tune

ivtv-tune: cmdline.o frequencies.o xawtv_parseconfig.o ivtv-tune.o

cmdline.c: ivtv-tune.ggo
	gengetopt --conf-parser -i $^

ivtv-tune.1: ivtv-tune
	help2man -N -o $@ ./$^
