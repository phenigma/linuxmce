ROOT = ..
CXXFLAGS = -Wall -O2 -D_GNU_SOURCE
CPPFLAGS = -I/usr/include/mysql -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG -DUSE_MYSQL
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lmysqlclient_r

sources = Database_pluto_security.cpp TableRow.cpp \
Table_Alert.cpp \
Table_AlertType.cpp \
Table_Alert_Device.cpp \
Table_ModeChange.cpp \
Table_Notification.cpp \
Table_psc_security_batdet.cpp \
Table_psc_security_bathdr.cpp \
Table_psc_security_batuser.cpp \
Table_psc_security_repset.cpp \
Table_psc_security_schema.cpp \
Table_psc_security_tables.cpp

all: libpluto_security.so all-recursive

libpluto_security.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	cp $@ ../lib

clean: clean-recursive
	rm -f *.o *.d libpluto_security.a >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
