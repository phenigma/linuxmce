#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread -lpluto_media -lattr

MYNAME = Powerfile_C200
VERSION = 2.0.0
MYDEVTEMPLATEID = 1737mplateID

Gen_Dev_Files = $(GENDEV)/Powerfile_C200Base.cpp
sources = Main.cpp Powerfile_C200.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += $(XXXFLAGS)
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = -lDisk_Drive_Functions

Gen_Dev_Files +=
sources += exec_grab_output.cpp ../JobHandler/Job.cpp ../JobHandler/Task.cpp ../Media_Plugin/MediaAttributes_LowLevel.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Powerfile_C200

so: Powerfile_C200.so

Powerfile_C200.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Powerfile_C200: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Powerfile_C200.bin Powerfile_C200.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

