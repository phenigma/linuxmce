ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib -L/usr/local/lib
LDLIBS= -lpluto_media -lpluto_main -lDCECommon -lSerializeClass -lmysqlclient_r -lPlutoUtils -lpthread -lattr -lid3

sources = Main.cpp \
UpdateMedia.cpp \
$(ROOT)/inotify/FileNotifier.cpp \
$(ROOT)/inotify/inotify_class.cpp \
$(ROOT)/UpdateMedia/id3info/id3info.cpp \
$(ROOT)/UpdateMedia/id3info/id3info_options.c

all: $(sources:.cpp=.o)
	$(CXX) -o UpdateMedia $+ $(LDFLAGS) $(LDLIBS)
	cp UpdateMedia $(ROOT)/bin/

clean:
	rm -f UpdateMedia $(sources:.cpp=.o) 
