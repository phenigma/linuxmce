#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -DDEBUG
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lpthread -lSerializeClass -lPlutoUtils -lDCECommon -lmysqlclient_r

MYNAME = Mozilla_Plugin
VERSION = 2.0.0
MYDEVTEMPLATEID = 30mplateID

Gen_Dev_Files = $(GENDEV)/Mozilla_PluginBase.cpp
sources = Main.cpp Mozilla_Plugin.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS +=
LDLIBS += -lPlutoUtils -lSerializeClass

Gen_Dev_Files += $(GENDEV)/Datagrid_PluginBase.cpp
sources += ../Datagrid_Plugin/Datagrid_Plugin.cpp

#<-dceag-mf-b->
whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Mozilla_Plugin

so: Mozilla_Plugin.so

Mozilla_Plugin.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Mozilla_Plugin: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) Mozilla_Plugin.bin Mozilla_Plugin.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/DCE/make.rules
-include $(sources:.cpp=.d)
#<-dceag-mf-e->

