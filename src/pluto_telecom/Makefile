ROOT = ..
CXXFLAGS = -Wall -O2 -D_GNU_SOURCE -fPIC -g -ggdb3
CPPFLAGS = -I/usr/include/mysql -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG -DUSE_MYSQL
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lmysqlclient_r

sources = Database_pluto_telecom.cpp TableRow.cpp \
Table_Contact.cpp \
Table_Contact_Users_Sync.cpp \
Table_IncomingLog.cpp \
Table_OutgoingLog.cpp \
Table_PhoneNumber.cpp \
Table_PhoneNumber_Users_Sync.cpp \
Table_PhoneType.cpp \
Table_SpeedDial.cpp \
Table_Users.cpp \
Table_psc_telecom_batdet.cpp \
Table_psc_telecom_bathdr.cpp \
Table_psc_telecom_batuser.cpp \
Table_psc_telecom_repset.cpp \
Table_psc_telecom_schema.cpp \
Table_psc_telecom_tables.cpp

all: libpluto_telecom.so all-recursive

libpluto_telecom.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	cp $@ ../lib

clean: clean-recursive
	rm -f *.o *.d libpluto_telecom.so >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
