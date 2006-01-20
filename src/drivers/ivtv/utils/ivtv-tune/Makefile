all: ivtv-tune

clean:
	rm -f *.o ivtv-tune

ivtv-tune: ivtv-tune.o frequencies.o xawtv_parseconfig.o cmdline.o

cmdline.c:
	gengetopt --conf-parser -i ivtv-tune.ggo

ivtv-tune.1: ivtv-tune
	help2man -N -o $@ ./$^
