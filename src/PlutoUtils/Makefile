SNR_CPPFLAGS ?= <-mkr_t_compile_defines->

ROOT=..
VIP = ../VIPShared
GDEV = ./Gen_Devices
CXXFLAGS = -fPIC -Wall -g -ggdb3 -D_GNU_SOURCE

have_httpfetcher = $(wildcard /usr/lib/libhttp_fetcher.*)

ifneq ($(strip $(have_httpfetcher)),)
	CXXFLAGS += -DUSE_HTTP_FETCHER
endif

CPPFLAGS = -I./ -I../../ -I../ -I../DCE/ -I../../DCE -I../VIPShared/ -I../../VIPShared -I/usr/include/mysql -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DUSE_LZO_DATAGRID $(SNR_CPPFLAGS)
CXX = g++
LDFLAGS = 

ifneq ($(strip $(have_httpfetcher)),)
	LDLIBS += -lhttp_fetcher
endif

sources = CommonIncludes.cpp FileUtils.cpp MultiThreadIncludes.cpp Other.cpp ProcessUtils.cpp StringUtils.cpp DatabaseUtils.cpp getch.cpp md5c.cpp minilzo.cpp uuencode.cpp LinuxSerialUSB.cpp ThreadedClass.cpp

all: libPlutoUtils.so all-recursive

libPlutoUtils.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../lib
	
libPlutoUtils.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	cp $@ ../lib
	
clean: 
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libPlutoUtils.so libPlutoUtils.a >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
