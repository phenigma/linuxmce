ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lDCECommon -lSerializeClass -lPlutoUtils

#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#<-mkr_b_use_mysql_wrapper_b->
CPPFLAGS += -DUSE_MYSQL_WRAPPER
#<-mkr_b_use_mysql_wrapper_e->


all: Main.o WatchGyroRemote.o UserUtils.o
	g++ Main.o WatchGyroRemote.o UserUtils.o -o WatchGyroRemote $(LDFLAGS) $(LDLIBS)
	cp WatchGyroRemote $(ROOT)/bin/

clean:
	rm -f WatchGyroRemote WatchGyroRemote.o
