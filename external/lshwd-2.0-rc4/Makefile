# simple makefile for lshwd
#
CFLAGS =-I. -Wall -O2 #-DDEBUG -ggdb3 #-D_GNU_SOURCE
INSTALL = install

OBJFILES = lshwd.c utilities.c usb.c psaux.c
#OBJFILES = lshwd.o utilities.o psaux.o

all: lshwd

lshwd: $(OBJFILES)

clean:
	rm -f *.o *~ lshwd

install:
	$(INSTALL) -D -m755 lshwd "$(DESTDIR)/usr/bin/lshwd"
	$(INSTALL) -D -m644 pcitable "$(DESTDIR)/usr/share/hwdata/pcitable"
#	$(INSTALL) -D -m644 pcmciatable "$(DESTDIR)/usr/share/hwdata/pcitable"
	$(INSTALL) -D -m644 usbtable "$(DESTDIR)/usr/share/hwdata/usbtable"
	$(INSTALL) -D -m644 Cards "$(DESTDIR)/usr/share/hwdata/Cards"	
