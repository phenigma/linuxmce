#<-dceag-mh-b->!
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
EXTERNAL_LIBS_NAMES = mysqlclient_r pthread
INTERNAL_LIBS_NAMES = SerializeClass DCECommon PlutoUtils

MYNAME = Xine_Player
VERSION = 2.0.0
MYDEVTEMPLATEID = 5mplateID

Gen_Dev_Files = $(GENDEV)/Xine_PlayerBase.cpp
sources = Main.cpp Xine_Player.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -fmessage-length=0
CPPFLAGS +=
LDFLAGS += -L/usr/X11R6/lib -L/usr/lib
INTERNAL_LIBS_NAMES +=
EXTERNAL_LIBS_NAMES += xine z nsl rt png X11 m Xext Xxf86vm

XTRALDLIBS =

Gen_Dev_Files +=
sources +=  XineSlaveWrapper.cpp 

#<-dceag-mf-b->!
LDLIBS_NAMES = $(INTERNAL_LIBS_NAMES) $(EXTERNAL_LIBS_NAMES)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Xine_Player

so: Xine_Player.so

Xine_Player.so: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.a) Makefile
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

Xine_Player: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.a) Makefile
	$(CXX) -o $(MYNAME) $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Xine_Player.bin Xine_Player.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

