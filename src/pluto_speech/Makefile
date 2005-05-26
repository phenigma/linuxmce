ROOT = ..
CXXFLAGS = -Wall -O2 -D_GNU_SOURCE
CPPFLAGS = -I/usr/include/mysql -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG -DUSE_MYSQL
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lmysqlclient_r

sources = Database_pluto_speech.cpp TableRow.cpp \
Table_Alias.cpp \
Table_Keyword.cpp

all: libpluto_speech.so all-recursive

libpluto_speech.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	cp $@ ../lib

clean: clean-recursive
	rm -f *.o *.d libpluto_speech.a >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
