#<-dceag-mh-b->!
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

CC = gcc
CFLAGS = $(CXXFLAGS) -m32 -DLINUX -ggdb3 -DIRTRANS_AS_LIB

MYNAME = IRTrans
VERSION = 2.0.0
MYDEVTEMPLATEID = 1710mplateID

Gen_Dev_Files = $(GENDEV)/IRTransBase.cpp
sources = Main.cpp IRTrans.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -DIRTRANS_AS_LIB
LDFLAGS +=
XTRALDLIBS =

Gen_Dev_Files +=
sources += ../LIRC_DCE/IRReceiverBase.cpp ../VFD_LCD/VFD_LCD_Base.cpp
Csources += fileio.c linuxserio.c lowlevel.c server.c errormessage.c flashrom.c webserver.c xap.c

#<-dceag-mf-b->!
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: IRTrans

so: IRTrans.so

IRTrans.so: $(sources:.cpp=.o) $(Csources:.c=.o) ccf.o
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
IRTrans: $(sources:.cpp=.o) $(Csources:.c=.o) ccf.o
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d) $(Csources:.c=.o) $(Csources:.c=.d)) IRTrans.bin IRTrans.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
-include $(Csources:.c=.d)
endif
#<-dceag-mf-e->

