PROGRAMS=interactor

CPPFLAGS=-g -ggdb3 -W -Wall
CXX=g++
CC=gcc

all: $(PROGRAMS)
	cp $(PROGRAMS) ../bin

interactor-server: interactor-server.o GetCommandOutput.o
	$(CXX) $(CPPFLAGS) -o interactor-server $+

clean:
	rm -f $(PROGRAMS) *.o
