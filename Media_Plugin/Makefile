#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -DDEBUG
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lpthread -lSerializeClass -lPlutoUtils -lDCECommon -lmysqlclient_r

MYNAME = Media_Plugin
VERSION = 2.0.0
MYDEVTEMPLATEID = 2mplateID

Gen_Dev_Files = $(GENDEV)/Media_PluginBase.cpp
sources = Main.cpp Media_Plugin.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS +=
LDLIBS += -lpluto_main -lPlutoUtils -lSerializeClass

Gen_Dev_Files += $(GENDEV)/Datagrid_PluginBase.cpp
sources += \
	../Datagrid_Plugin/Datagrid_Plugin.cpp \
	../File_Grids_Plugin/FileListGrid.cpp \
	MediaPluginInfo.cpp \
	MediaAttributes.cpp
	

#<-dceag-mf-b->
whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Media_Plugin

so: Media_Plugin.so

Media_Plugin.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $+ $(LDFLAGS) $(LDLIBS)
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

include $(ROOT)/DCE/make.rules
-include $(sources:.cpp=.d)
#<-dceag-mf-e->

