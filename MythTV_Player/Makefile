#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
MYNAME = MythTV_Player
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -DDEBUG
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lpthread -lDCECommon -lmysqlclient_r

VERSION = 2.0.0

Gen_Dev_Files = $(GENDEV)/MythTV_PlayerBase.cpp
sources = Main.cpp MythTV_Player.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/qt3 -I/home/work/mythtv/libs/libmythtv -DQT_NO_DEBUG -DQT_THREAD_SUPPORT -DQT_SHARED
LDFLAGS +=
LDLIBS += -lqt-mt -lmyth-0.16 -lmythtv-0.16

Gen_Dev_Files +=
sources += 

#<-dceag-mf-b->
whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: MythTV_Player

so: MythTV_Player.so

MythTV_Player.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
MythTV_Player: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) MythTV_Player.bin MythTV_Player.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION)

include $(ROOT)/DCE/make.rules
-include $(sources:.cpp=.d)
#<-dceag-mf-e->

