ROOT=..
CXXFLAGS = -Wall -ggdb3
CPPFLAGS = -I./ -I../ -I../DCE/ -I/usr/include/mysql
CXX = g++
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lpthread -lDCECommon -lSerial -lSerializeClass -lPlutoUtils -lMessageTranslation

APPNAME = IRBase
VERSION = 2.0.0

sources = IRBase.cpp

all: libIRBase all-recursive

libIRBase: libIRBase.so libIRBase.a

libIRBase.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
#	cp $@ $(ROOT)/lib

libIRBase.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	cp $@ $(ROOT)/lib

clean:
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libIRBase.so libIRBase.a >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
