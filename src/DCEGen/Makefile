ROOT = ..
CXXFLAGS = -pipe -Wall -ggdb3
CPPFLAGS = -I/usr/local/include -I$(ROOT)/DCE -I.. -I. -I/usr/include/mysql
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -ldl -lmysqlclient -lpthread -lpluto_main -lDCECommon -lSerializeClass -lPlutoUtils

APPNAME = DCEGen
VERSION = 2.0.0

sources = DCEGen.cpp

all: $(sources:.cpp=.o)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o DCEGen $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp DCEGen ../bin

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o)) $(call cpath,$(sources:.cpp=.d)) DCEGen >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
