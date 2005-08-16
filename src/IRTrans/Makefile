SHELL = /bin/sh

CC = gcc

CFLAGS = -m32
CPPFLAGS = -DLINUX -ggdb3 -DIRTRANS_AS_LI
LDFLAGS =
LIBS =

IRSERVER_OBJS = fileio.o linuxserio.o lowlevel.o server.o errormessage.o flashrom.o webserver.o xap.o

irserver: $(IRSERVER_OBJS) ccf.o 
	$(CC) $(CFLAGS) $(IRSERVER_OBJS) ccf.o -o irserver $(LDFLAGS)

clean:
	rm $(IRSERVER_OBJS)

%.o: %.c dbstruct.h fileio.h lowlevel.h ../Common/network.h serio.h pictures.h ../Common/remote.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

irclient: client.c
	$(CC) $(CFLAGS) client.c -o irclient $(LDFLAGS)
