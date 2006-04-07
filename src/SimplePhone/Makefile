#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

MYNAME = SimplePhone
VERSION = 2.0.0
MYDEVTEMPLATEID = 1759mplateID

Gen_Dev_Files = $(GENDEV)/SimplePhoneBase.cpp
sources = Main.cpp SimplePhone.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -Ilib
CPPFLAGS += -Ilib
LDFLAGS += -L./lib
XTRALDLIBS = -liaxclient -lm -lasound

Gen_Dev_Files +=
sources +=

#<-dceag-mf-b->!
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so 

bin: lib/libiaxclient.a SimplePhone 

so: SimplePhone.so

SimplePhone.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
SimplePhone: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) SimplePhone.bin SimplePhone.so >/dev/null
	make -C lib clean

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

lib/libiaxclient.a:
	make -C lib
