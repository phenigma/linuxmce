ROOT = ../
CXXFLAGS = -Wall -g -static
CPPFLAGS = -I$(ROOT) -I.. -I$(ROOT)/DCE
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -lpthread -lDCECommon -lPlutoUtils -lSerializeClass

APPNAME = MessageSend
VERSION = 2.0.0

sources = MessageSend.cpp ParamGet.cpp

all: MessageSend 
	cp $+ $(ROOT)/bin

MessageSend: MessageSend.o
	$(CXX) -o $@ $+  $(LDFLAGS) $(LDLIBS)

clean: clean-recursive
	rm -f *.o *.d MessageSend >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/DCE/make.rules
-include $(sources:.cpp=.d)
