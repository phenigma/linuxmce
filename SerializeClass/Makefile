ROOT = ..
VIP = ../VIPShared
GDEV = ./Gen_Devices
CXXFLAGS = -Wall -g -ggdb3 -D_GNU_SOURCE -DDEBUG
CPPFLAGS = -I./ -I../../ -I../ -I../DCE/ -I../../DCE -I../VIPShared/ -I../../VIPShared -I/usr/include/mysql -DXLL_DEBUG_FILE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DXTHREAD_LOG -DDEBUG -DUSE_LZO_DATAGRID
CXX = g++

sources = SerializeClass.cpp

all: libSerializeClass all-recursive

libSerializeClass: libSerializeClass.so libSerializeClass.a

libSerializeClass.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $+
#	cp $@ ../lib
	
libSerializeClass.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	cp $@ ../lib
	
clean: 
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) libSerializeClass.so libSerializeClass.a >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
