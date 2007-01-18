all: cx25840ctl

clean:
	rm -f *.o cx25840ctl

cx25840ctl: cx25840ctl.o cx25840-registers.o cmdline.o

cmdline.c:
	gengetopt -u -i cx25840ctl.ggo

cx25840ctl.1: cx25840ctl
	help2man -N -o $@ ./$^
