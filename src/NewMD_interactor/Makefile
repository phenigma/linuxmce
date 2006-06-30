PROGRAMS=interactor interactor-server

CPPFLAGS=-g -ggdb3 -W -Wall
CXX=g++-3.3
CC=gcc-3.3

all: $(PROGRAMS)
	cp $(PROGRAMS) ../bin

interactor-server: interactor-server.o GetCommandOutput.o
	$(CXX) $(CPPFLAGS) -o interactor-server $+

clean:
	rm -f $(PROGRAMS) *.o
