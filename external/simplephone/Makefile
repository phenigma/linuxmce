CC       = gcc
LINK	 = gcc
INCPATH  = -I. -Ilib
LIBS     = -Llib -liaxclient -lm -lpthread
CFLAGS   = -Wall -pedantic -g -O2
SRC	 = simplephone.c
OBJ	 = simplephone.o
BIN	 = simplephone
SCRIPT   = simplephone.pl

all:	$(BIN)

$(BIN): lib/libiaxclient.a $(OBJ) 
	$(LINK) -o $(BIN) $(OBJ) $(LIBS)

clean:
	make -C lib clean
	rm -rf $(OBJ) $(BIN)

lib/libiaxclient.a:
	make -C lib
	
$(OBJ): $(SRC)
	$(CC) -c $(CFLAGS) $(INCPATH) $(SRC)

install: $(BIN) $(SCRIPT)
	mkdir -p $(DESTDIR)/usr/pluto/bin
	strip $(BIN)
	cp $(BIN) $(SCRIPT) $(DESTDIR)/usr/pluto/bin
