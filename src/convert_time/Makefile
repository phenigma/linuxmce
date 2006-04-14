ROOT = ..
CXXFLAGS = -Wall -g 
CPPFLAGS = -I$(ROOT)/ -I$(ROOT)/DCE -D_REENTRANT -D_GNU_SOURCE -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib 
LDLIBS = 
CXX = g++

sources = convert_time.cpp

XTRALDLIBS = 

all: convert_time

convert_time: $(sources:.cpp=.o)
	$(CXX) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ $(ROOT)/bin

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) convert_time >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
