ROOT=..
CXXFLAGS = -Wall -g -D_GNU_SOURCE
CPPFLAGS = -I./ -I../../ -I../ -I/usr/include/mysql -I$(ROOT) -I$(ROOT)/DCE -I$(ROOT)/BDBluetoothCommands -I$(ROOT)/BDSymbianCommands -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DUSE_MYSQL -DVIPSERVER -DXBTSOCKET -DBLUETOOTH_DONGLE
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -lmysqlclient_r

sources = BDCommand.cpp BDCommandProcessor.cpp

all: libBDCommon.a all-recursive

libBDCommon.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	cp $@ ../lib
		
clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) libBDCommon.a >/dev/null

include ./make.rules
-include $(sources:.cpp=.d)
