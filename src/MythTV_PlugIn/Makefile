#<-dceag-mh-b->!
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
EXTERNAL_LIBS_NAMES = mysqlclient_r pthread
INTERNAL_LIBS_NAMES = SerializeClass DCECommon PlutoUtils

MYNAME = MythTV_PlugIn
VERSION = 2.0.0
MYDEVTEMPLATEID = 36mplateID

Gen_Dev_Files = $(GENDEV)/MythTV_PlugInBase.cpp
sources = Main.cpp MythTV_PlugIn.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/qt3 -I$(ROOT)/../libs/Libraries
INTERNAL_LIBS_NAMES += pluto_main pluto_media
EXTERNAL_LIBS_NAMES += qt-mt myth-0.16 mythtv-0.16

Gen_Dev_Files += $(GENDEV)/Media_PluginBase.cpp $(GENDEV)/Datagrid_PluginBase.cpp
sources += \
	../Orbiter_Plugin/FollowMe_Plugin.cpp \
	../Media_Plugin/MediaHandlerInfo.cpp \
	../Media_Plugin/MediaHandlerBase.cpp \
	../Media_Plugin/Media_Plugin.cpp \
	../Media_Plugin/MediaAttributes.cpp \
	../Media_Plugin/MediaStream.cpp \
	../Media_Plugin/BoundRemote.cpp \
	../Datagrid_Plugin/Datagrid_Plugin.cpp \
	../File_Grids_Plugin/FileListGrid.cpp \
	MythTvMediaStream.cpp \
	MythTvWrapper.cpp

#<-dceag-mf-b->!
LDLIBS_NAMES = $(INTERNAL_LIBS_NAMES) $(EXTERNAL_LIBS_NAMES)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: MythTV_PlugIn

so: MythTV_PlugIn.so

MythTV_PlugIn.so: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.a) Makefile
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

MythTV_PlugIn: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.a) Makefile
	$(CXX) -o $(MYNAME) $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) MythTV_PlugIn.bin MythTV_PlugIn.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

