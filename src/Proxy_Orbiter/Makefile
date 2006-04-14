#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

MYNAME = Proxy_Orbiter
VERSION = 2.0.0
MYDEVTEMPLATEID = 1727mplateID

Gen_Dev_Files = $(GENDEV)/OrbiterBase.cpp
sources = Main.cpp Proxy_Orbiter.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/SDL -DORBITER -DPROXY_ORBITER
LDFLAGS +=
XTRALDLIBS = -lSDL -lSDL_ttf -lSDL_image -lSGE -lSDL_gfx -lcurl

sources += \
	../Orbiter/OrbiterFileBrowser.cpp \
	../Orbiter/Simulator.cpp \
	../Orbiter/DesignObj_Data.cpp \
	../Orbiter/DesignObj_Orbiter.cpp \
	../Orbiter/Orbiter.cpp \
	../Orbiter/GraphicBuilder.cpp \
	../Orbiter/RendererMNG.cpp \
	../Orbiter/TextStyle.cpp \
	../Orbiter/CacheImageManager.cpp \
	../Orbiter/RendererOCG.cpp \
	../Orbiter/ScreenHandler.cpp \
	../Orbiter/ScreenHistory.cpp \
	../Orbiter/SDL/SDLRendererOCGHelper.cpp \
	../Orbiter/SDL/SDLGraphic.cpp \
	../Orbiter/SDL/OrbiterSDL.cpp \
	../Orbiter/SDL/JpegWrapper.cpp \
	../Orbiter/MouseBehavior.cpp \
	../Orbiter/MouseGovernor.cpp \
	../Orbiter/MouseIterator.cpp \
	../Orbiter/CallBackData.cpp \
	../Splitter/TextWrapper.cpp \
	../Splitter/TextSplitter.cpp \
	../OrbiterGen/Renderer.cpp \
	../PlutoUtils/HttpUtils.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Proxy_Orbiter

so: Proxy_Orbiter.so

Proxy_Orbiter.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Proxy_Orbiter: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Proxy_Orbiter.bin Proxy_Orbiter.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

