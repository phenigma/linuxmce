#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
MYNAME = Xine_Plugin
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -DDEBUG
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lpthread -lDCECommon -lmysqlclient_r

VERSION = 2.0.0

Gen_Dev_Files = $(GENDEV)/Xine_PluginBase.cpp
sources = Main.cpp Xine_Plugin.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -fmessage-length=0
CPPFLAGS +=
LDFLAGS +=
LDLIBS += -lpluto_main -lPlutoUtils -lSerializeClass

Gen_Dev_Files += $(GENDEV)/Media_PluginBase.cpp $(GENDEV)/Datagrid_PluginBase.cpp
sources += ../Media_Plugin/MediaPluginInfo.cpp ../Media_Plugin/Media_Plugin.cpp ../Datagrid_Plugin/Datagrid_Plugin.cpp

#<-dceag-mf-b->
whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Xine_Plugin

so: Xine_Plugin.so

Xine_Plugin.so: $(sources:.cpp=.o) Makefile
	$(CXX) -shared -o $(MYNAME).so $(sources:.cpp=.o) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Xine_Plugin: $(sources:.cpp=.o) Makefile
	$(CXX) -o $(MYNAME) $(sources:.cpp=.o) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) Xine_Plugin.bin Xine_Plugin.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION)

include $(ROOT)/DCE/make.rules
-include $(sources:.cpp=.d)
#<-dceag-mf-e->

