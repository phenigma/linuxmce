ROOT = ..
CXXFLAGS = -Wall -g 
CPPFLAGS = -I$(ROOT)/ -I$(ROOT)/DCE -I$(ROOT)/VIPShared -D_REENTRANT -D_GNU_SOURCE -DDEBUG -DNOBLUETOOTH
LDFLAGS = -L$(ROOT)/lib 
LDLIBS = -lDCECommon -lattr
CXX = g++

sources = 	convert_mac.cpp

all: convert_mac

convert_mac: $(sources:.cpp=.o)
	$(CXX) -o $@ $+  $(LDFLAGS) $(LDLIBS)
	cp $@ $(ROOT)/bin

clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) convert_mac >/dev/null

include $(ROOT)/DCE/make.rules
-include $(sources:.cpp=.d)
