ROOT=..
VIP = ../VIPShared
GDEV = ./Gen_Devices
CXXFLAGS = -Wall -g -ggdb3 -D_GNU_SOURCE -DDEBUG
CPPFLAGS = -I./ -I../../ -I../ -I../DCE/ -I../../DCE -I../VIPShared/ -I../../VIPShared -I/usr/include/mysql -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DUSE_LZO_DATAGRID -DDEBUG -DTHREAD_LOG -DLL_DEBUG_FILE
CXX = g++

sources = CommonIncludes.cpp FileUtils.cpp MultiThreadIncludes.cpp Other.cpp StringUtils.cpp getch.cpp md5c.cpp minilzo.cpp

all: libPlutoUtils all-recursive

libPlutoUtils: libPlutoUtils.so libPlutoUtils.a

libPlutoUtils.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
#	cp $@ ../lib
	
libPlutoUtils.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	cp $@ ../lib
	
clean: 
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libPlutoUtils.so libPlutoUtils.a >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
