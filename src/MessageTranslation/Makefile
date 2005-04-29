ROOT=..
CXXFLAGS = -Wall -ggdb3
CPPFLAGS = -I./ -I../ -I../DCE/ -I/usr/include/mysql
CXX = g++
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lpthread -lDCECommon -lSerial -lSerializeClass -lPlutoUtils

APPNAME = IRBase
VERSION = 2.0.0

sources = MessageTranslation.cpp DefaultMessageTranslation.cpp AVMessageTranslation.cpp

all: libMessageTranslation all-recursive

libMessageTranslation: libMessageTranslation.so libMessageTranslation.a

libMessageTranslation.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
#	cp $@ $(ROOT)/lib

libMessageTranslation.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	cp $@ $(ROOT)/lib

clean:
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libMessageTranslation.so libMessageTranslation.a >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
