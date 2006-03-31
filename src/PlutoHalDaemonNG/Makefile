ROOT = ..
CXXFLAGS = -Wall -g -pipe 
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -lpthread -ldbus-glib-1 -lgthread-2.0 -ldbus-1 -lglib-2.0 -lhal
INCL= -I/usr/include/dbus-1.0/ -I/usr/include/hal -I/usr/lib/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

APPNAME = pluto-hald
VERSION = 1.0.0

sources = pluto-hald.cpp
CPPFLAGS = $(INCL) -DDBUS_API_SUBJECT_TO_CHANGE 

all: pluto-hald 
	cp $+ $(ROOT)/bin

pluto-hald: $(sources:.cpp=.o)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $(call cpath,$+)  $(LDFLAGS) $(LDLIBS)

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) PlutoHalDaemon >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

