#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

MYNAME = Orbiter_Plugin
VERSION = 2.0.0
MYDEVTEMPLATEID = 12mplateID

Gen_Dev_Files = $(GENDEV)/Orbiter_PluginBase.cpp
sources = Main.cpp Orbiter_Plugin.cpp PopulateListsInVMC.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -fmessage-length=0
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = -lpluto_main

Gen_Dev_Files +=
sources += FollowMe_Plugin.cpp ../CreateDevice/CreateDevice.cpp ../VIPShared/VIPMenu.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Orbiter_Plugin

so: Orbiter_Plugin.so

Orbiter_Plugin.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Orbiter_Plugin: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) Orbiter_Plugin.bin Orbiter_Plugin.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

