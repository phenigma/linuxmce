#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

MYNAME = Xine_Plugin
VERSION = 2.0.0
MYDEVTEMPLATEID = 6mplateID

Gen_Dev_Files = $(GENDEV)/Xine_PluginBase.cpp
sources = Main.cpp Xine_Plugin.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -fmessage-length=0
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = -lpluto_main -lpluto_media -lattr

Gen_Dev_Files += $(GENDEV)/Media_PluginBase.cpp $(GENDEV)/Datagrid_PluginBase.cpp
sources += \
	XineMediaStream.cpp \
	../Orbiter_Plugin/FollowMe_Plugin.cpp \
	../Media_Plugin/MediaHandlerInfo.cpp \
	../Media_Plugin/MediaHandlerBase.cpp \
	../Media_Plugin/Media_Plugin.cpp \
	../Media_Plugin/MediaAttributes.cpp \
	../Media_Plugin/MediaStream.cpp \
	../Media_Plugin/BoundRemote.cpp \
	../Datagrid_Plugin/Datagrid_Plugin.cpp \
	../File_Grids_Plugin/FileListGrid.cpp

#<-dceag-mf-b->!
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Xine_Plugin

so: Xine_Plugin.so

Xine_Plugin.so: $(sources:.cpp=.o) $(ROOT)/lib/libSerializeClass.a $(ROOT)/lib/libDCECommon.a $(ROOT)/lib/libpluto_main.a $(ROOT)/lib/libpluto_media.a $(ROOT)/lib/libPlutoUtils.a
	$(CXX) -shared -o $(MYNAME).so $(sources:.cpp=.o) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

Xine_Plugin: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) Xine_Plugin.bin Xine_Plugin.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
#<-dceag-mf-e->

