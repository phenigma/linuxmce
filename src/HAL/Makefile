#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

MYNAME = HAL
VERSION = 2.0.0
MYDEVTEMPLATEID = 1797mplateID

Gen_Dev_Files = $(GENDEV)/HALBase.cpp
sources = Main.cpp HAL.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -I/usr/include/dbus-1.0/ -I/usr/include/hal \
	-I/usr/lib/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include \
	-DDBUS_API_SUBJECT_TO_CHANGE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	-D_DEVEL_DEFINES -DUSE_LZO_DATAGRID -D_GNU_SOURCE  -Wno-deprecated
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = -ldbus-glib-1 -lgthread-2.0 -ldbus-1 -lglib-2.0 -lhal

Gen_Dev_Files +=
sources += pluto-hald.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: HAL

so: HAL.so

HAL.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
HAL: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) HAL.bin HAL.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

