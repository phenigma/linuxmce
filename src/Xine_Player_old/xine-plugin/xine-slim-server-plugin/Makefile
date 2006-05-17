sources=slim-input-plugin.c slim-protocol-handler.c slim-stream.c socket-utils.c

LIBTOOL=libtool
CC=gcc
CFLAGS=`xine-config --cflags` -ggdb3
LDFLAGS=-fPIC

xineplug_inp_slim.so: $(sources:.c=.o)
	$(LIBTOOL) --mode=link --tag=CC $(CC) -shared -avoid-version  $(CFLAGS) $(LDFLAGS) -o $@ $(call cpath,$+) `xine-config --libs`
# 	gcc -o xineplug_inp_slim.so -module -avoid-version $(call cpath,$(sources:.c=.o)) `xine-config --libs`

.PHONY: install clean
install: xineplug_inp_slim.so
	cp xineplug_inp_slim.so `xine-config --plugindir`/

clean:
	rm -f *.o *.so

include $(ROOT)/PlutoUtils/make.rules
