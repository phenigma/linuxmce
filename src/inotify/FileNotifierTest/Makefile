ROOT = ../..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYNAME = FileNotifierTest
VERSION = 2.0.0
MYDEVTEMPLATEID = 8mplateID

sources = FileNotifierTest.cpp ../FileNotifier.cpp ../inotify_class.cpp

CXXFLAGS += -g -O0
CPPFLAGS += 
LDFLAGS += 
XTRALDLIBS = -lDCECommon 

LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make all'"

all: bin

bin: FileNotifierTest

FileNotifierTest: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDFLAGS) $(LDLIBS)

clean: 
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) FileNotifierTest.bin > /dev/null


