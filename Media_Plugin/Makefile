#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

MYNAME = Media_Plugin
VERSION = 2.0.0
MYDEVTEMPLATEID = 2mplateID

Gen_Dev_Files = $(GENDEV)/Media_PluginBase.cpp
sources = Main.cpp Media_Plugin.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -fmessage-length=0
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = -lpluto_main -lpluto_media

Gen_Dev_Files += $(GENDEV)/Datagrid_PluginBase.cpp
sources += \
	../Datagrid_Plugin/Datagrid_Plugin.cpp \
	../File_Grids_Plugin/FileListGrid.cpp \
	MediaPluginInfo.cpp \
	MediaAttributes.cpp
	

#<-dceag-mf-b->!
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Media_Plugin

so: Media_Plugin.so

Media_Plugin.so: $(sources:.cpp=.o) $(ROOT)/lib/libpluto_main.a $(ROOT)/lib/libpluto_media.a $(ROOT)/lib/libDCECommon.a $(ROOT)/lib/libSerializeClass.a $(ROOT)/lib/libPlutoUtils.a
	$(CXX) -shared -o $(MYNAME).so $(sources:.cpp=.o) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Media_Plugin: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) Media_Plugin.bin Media_Plugin.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
#<-dceag-mf-e->

