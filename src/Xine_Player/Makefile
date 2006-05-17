#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread -ljpeg -lxine -lXtst -lX11

MYNAME = Xine_Player
VERSION = 2.0.0
MYDEVTEMPLATEID = 5mplateID

Gen_Dev_Files = $(GENDEV)/Xine_PlayerBase.cpp
sources = Main.cpp Xine_Player.cpp Colorspace_Utils.cpp JpegEncoderDecoder.cpp Xine_Stream_Factory.cpp Xine_Stream.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS += -L/usr/X11R6/lib  
XTRALDLIBS =

Gen_Dev_Files +=
sources +=

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Xine_Player

so: Xine_Player.so

Xine_Player.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Xine_Player: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Xine_Player.bin Xine_Player.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

