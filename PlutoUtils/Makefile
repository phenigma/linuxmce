ROOT=..
VIP = ../VIPShared
GDEV = ./Gen_Devices
CXXFLAGS = -Wall -g -ggdb3 -D_GNU_SOURCE -DDEBUG
CPPFLAGS = -I./ -I../../ -I../ -I../DCE/ -I../../DCE -I../VIPShared/ -I../../VIPShared -I/usr/include/mysql -DXLL_DEBUG_FILE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DXTHREAD_LOG -DDEBUG -DUSE_LZO_DATAGRID
CXX = g++

sources = CommonIncludes.cpp FileUtils.cpp MultiThreadIncludes.cpp Other.cpp StringUtils.cpp getch.cpp md5c.cpp minilzo.cpp

all: libPlutoUtils all-recursive

libPlutoUtils: libPlutoUtils.so libPlutoUtils.a

libPlutoUtils.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $+
#	cp $@ ../lib
	
libPlutoUtils.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	cp $@ ../lib
	
clean: 
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) libPlutoUtils.so libPlutoUtils.a >/dev/null

include $(ROOT)/DCE/make.rules
-include $(sources:.cpp=.d)
