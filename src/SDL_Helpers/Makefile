ROOT = ..
VIP = ../VIPShared
GDEV = ./Gen_Devices
CXXFLAGS = -Wall -g -ggdb3 -D_GNU_SOURCE
CPPFLAGS = -I./ -I$(ROOT) -I/usr/include/SDL -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE #<-mkr_t_compile_defines->
CXX = g++
LDFLAGS =
LDLIBS = -lSDL -lpng

sources = SDL_CompositeAlphaChannel.cpp SDL_pixel.cpp SDL_SavePNG.cpp

all: libSDL_Helpers.so

libSDL_Helpers.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../lib
	
libSDL_Helpers.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	cp $@ ../lib
	
clean: 
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libSDL_Helpers.so libSDL_Helpers.a >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
