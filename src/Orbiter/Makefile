#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

MYNAME = Orbiter
VERSION = 2.0.0
MYDEVTEMPLATEID = 8mplateID

Gen_Dev_Files = $(GENDEV)/OrbiterBase.cpp
sources = Main.cpp Orbiter.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/SDL -DORBITER -DCACHE_IMAGES -DDEBUG -DxPHONEKEYS -DxTHREAD_LOG -I$(ROOT)/Splitter -I$(ROOT)/OrbiterGen -L/usr/X11/lib
LDFLAGS += -L/usr/X11R6/lib
XTRALDLIBS = -lDCECommon -lSDL -lSDL_ttf -lSDL_image -lSGE -lSDL_gfx -lXtst

Gen_Dev_Files +=
sources +=  DesignObj_Data.cpp DesignObj_Orbiter.cpp \
			Simulator.cpp \
			Linux/OrbiterLinux.cpp \
			Linux/XRecordExtensionHandler.cpp \
			RendererMNG.cpp \
			GraphicBuilder.cpp \
			TextStyle.cpp \
			CacheImageManager.cpp \
			SDL/SDLGraphic.cpp \
			SDL/OrbiterSDL.cpp SDL/StartOrbiterSDL.cpp \
			$(ROOT)/Splitter/TextSplitter.cpp \
			$(ROOT)/Splitter/TextWrapper.cpp \
			$(ROOT)/OrbiterGen/Renderer.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Orbiter

so: Orbiter.so

Orbiter.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Orbiter: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Orbiter.bin Orbiter.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

