ROOT=..
CXXFLAGS = -Wall -g -D_GNU_SOURCE
CPPFLAGS = -I../../ -I../ -I/usr/include/mysql -I$(ROOT) -I$(ROOT)/DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DUSE_MYSQL -DVIPSERVER -DBTSOCKET
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -lmysqlclient_r

sources = RA_Processor.cpp RA_Action.cpp RA_Request.cpp RAServer.cpp RAClientSocket.cpp RAServerSocket.cpp

all: libRACommon.a all-recursive

libRACommon.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	cp $@ ../lib
		
clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) libRACommon.a >/dev/null

include ./make.rules
-include $(sources:.cpp=.d)
