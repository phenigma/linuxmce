ROOT = ..
CXXFLAGS = -Wall -O2 -D_GNU_SOURCE
CPPFLAGS = -I/usr/include/mysql -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG -DUSE_MYSQL
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lmysqlclient_r

sources = Database_pluto_media.cpp TableRow.cpp \
Table_Attribute.cpp \
Table_AttributeType.cpp \
Table_Disc.cpp \
Table_Disc_Attribute.cpp \
Table_File.cpp \
Table_File_Attribute.cpp \
Table_Picture.cpp \
Table_Picture_Attribute.cpp \
Table_Picture_File.cpp \
Table_Playlist.cpp \
Table_PlaylistEntry.cpp \
Table_SearchToken.cpp \
Table_SearchToken_Attribute.cpp \
Table_Type.cpp \
Table_Type_AttributeType.cpp \
Table_Type_Extension.cpp \
Table_psc_media_batdet.cpp \
Table_psc_media_bathdr.cpp \
Table_psc_media_batuser.cpp \
Table_psc_media_repset.cpp \
Table_psc_media_schema.cpp \
Table_psc_media_tables.cpp

all: libpluto_media.a all-recursive

libpluto_media.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	cp $@ ../lib

clean: clean-recursive
	rm -f *.o *.d libpluto_media.a >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
