ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-l pluto_main -lDCECommon -lSerializeClass -lmysqlclient_r -lPlutoUtils

all: Main.o UpdateEntArea.o UpdateEntArea_Climate.o UpdateEntArea_Lighting.o UpdateEntArea_Media.o UpdateEntArea_Security.o UpdateEntArea_Telecom.o UpdateEntArea_Setup.o UpdateEntArea_Event.o CommandGroupArray.o Criteria.o
	g++ Main.o UpdateEntArea.o UpdateEntArea_Climate.o UpdateEntArea_Lighting.o UpdateEntArea_Media.o UpdateEntArea_Security.o UpdateEntArea_Telecom.o UpdateEntArea_Setup.o UpdateEntArea_Event.o CommandGroupArray.o Criteria.o -o UpdateEntArea $(LDFLAGS) $(LDLIBS)
	cp UpdateEntArea $(ROOT)/bin/

clean:
	rm -f UpdateEntArea UpdateEntArea.o
