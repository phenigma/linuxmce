ROOT = ..
CXXFLAGS = -Wall -ggdb3 
CPPFLAGS = -I./ -I../ -I../DCE/ -I../VIPShared/ -I/usr/include/mysql
CXX = g++

APPNAME = Serial
VERSION = 2.0.0

sources = SerialPort.cpp

all: Serial all-recursive

Serial: libSerial.so libSerial.a

libSerial.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $+
#	cp $@ ../lib
	
libSerial.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	cp $@ ../lib
	
clean: 
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) libSerial.so libSerial.a >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
