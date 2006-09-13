ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -I$(ROOT)/inotify
LDFLAGS=-L$(ROOT)/lib -L/usr/local/lib
LDLIBS= -lpluto_main -lDCECommon -lSerializeClass -lmysqlclient_r -lPlutoUtils -lpthread -lid3

sources = Main.cpp \
$(ROOT)/inotify/FileNotifier.cpp \
$(ROOT)/inotify/inotify_class.cpp \

all: $(sources:.cpp=.o)
	$(CXX) -o VoiceMailMonitor $+ $(LDFLAGS) $(LDLIBS)
	cp VoiceMailMonitor $(ROOT)/bin/

clean:
	rm -f VoiceMailMonitor $(sources:.cpp=.o) 
