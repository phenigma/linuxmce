ROOT = ..
CXXFLAGS = -Wall -O2 -D_GNU_SOURCE
CPPFLAGS = -I/usr/include/mysql -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG -DUSE_MYSQL
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lmysqlclient_r

sources = Database_pluto_security.cpp TableRow.cpp \
Table_Alert.cpp \
Table_AlertType.cpp \
Table_ModeChange.cpp \
Table_Notification.cpp \
Table_Picture.cpp

all: libpluto_security.a all-recursive

libpluto_security.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	cp $@ ../lib

clean: clean-recursive
	rm -f *.o *.d libpluto_security.a >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
