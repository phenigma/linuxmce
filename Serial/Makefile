CXXFLAGS = -Wall -ggdb3 
CPPFLAGS = -I./ -I../ -I../DCE/ -I../VIPShared/ -I/usr/include/mysql
CXX = g++

APPNAME = Serial
VERSION = 2.0.0

sources = SerialPort.cpp

all: Serial all-recursive

Serial: Serial.so Serial.a

Serial.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $+
#	cp $@ ../lib
	
Serial.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	cp $@ ../lib
	
clean: 
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) Serial.so Serial.a >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include ../DCE/make.rules
-include $(sources:.cpp=.d)
