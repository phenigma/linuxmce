#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -DDEBUG
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lpthread -lSerializeClass -lPlutoUtils -lDCECommon -lmysqlclient_r

MYNAME = MythTV_PlugIn
VERSION = 2.0.0
MYDEVTEMPLATEID = 36mplateID

Gen_Dev_Files = $(GENDEV)/MythTV_PlugInBase.cpp
sources = Main.cpp MythTV_PlugIn.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/qt3 -I/home/work/mythtv/libs/libmythtv
LDFLAGS += 
LDLIBS += -lpluto_main -lFakeEPG -lPlutoUtils -lSerializeClass

Gen_Dev_Files += $(GENDEV)/Media_PluginBase.cpp $(GENDEV)/Datagrid_PluginBase.cpp
sources += ../Media_Plugin/MediaPluginInfo.cpp ../Media_Plugin/Media_Plugin.cpp ../Datagrid_Plugin/Datagrid_Plugin.cpp MythTvWrapper.cpp 

#<-dceag-mf-b->
whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: MythTV_PlugIn

so: MythTV_PlugIn.so

MythTV_PlugIn.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
MythTV_PlugIn: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) MythTV_PlugIn.bin MythTV_PlugIn.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/DCE/make.rules
-include $(sources:.cpp=.d)
#<-dceag-mf-e->

