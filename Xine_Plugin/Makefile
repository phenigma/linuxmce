#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -DDEBUG
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lpthread -lSerializeClass -lPlutoUtils -lDCECommon -lmysqlclient_r

MYNAME = Xine_Plugin
VERSION = 2.0.0
MYDEVTEMPLATEID = 6mplateID

Gen_Dev_Files = $(GENDEV)/Xine_PluginBase.cpp
sources = Main.cpp Xine_Plugin.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -fmessage-length=0
CPPFLAGS +=
LDFLAGS +=
LDLIBS += -lpluto_main -lPlutoUtils -lSerializeClass

Gen_Dev_Files += $(GENDEV)/Media_PluginBase.cpp $(GENDEV)/Datagrid_PluginBase.cpp
sources += \
	../Media_Plugin/MediaPluginInfo.cpp \
	../Media_Plugin/Media_Plugin.cpp \
	../Media_Plugin/MediaAttributes.cpp \
	../Datagrid_Plugin/Datagrid_Plugin.cpp \
	../File_Grids_Plugin/FileListGrid.cpp

#<-dceag-mf-b->
whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Xine_Plugin

so: Xine_Plugin.so

Xine_Plugin.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $+ $(LDFLAGS) $(LDLIBS)
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

include $(ROOT)/DCE/make.rules
-include $(sources:.cpp=.d)
#<-dceag-mf-e->

