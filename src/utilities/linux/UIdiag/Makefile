CXX=g++
CC=$(CXX)
CXXFLAGS=-W -Wall -pipe -ggdb3
CPPFLAGS=-I. -I/usr/include/SDL -I/usr/X11R6/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
LDFLAGS=-lXtst -lGL -lGLU -lXrender -lXext -lSDL -lSDL_ttf -lSDL_image

sources = UIdiag.cpp
			
all: $(sources:.cpp=.o)
	$(CXX) -o UIdiag $+ $(LDFLAGS)

clean:
	rm -f UIdiag $(sources:.cpp=.o) 
	
