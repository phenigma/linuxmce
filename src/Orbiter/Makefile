# if ( USE_WX_LIB > 0 )
#   compile & link with wx
# else
#   not using wx at all

USE_WX_LIB = 0
ifneq ($(USE_WX_LIB), 0)
USE_WX_LIB := 1
endif

ifneq ($(USE_WX_LIB), 0)
# using wx

# wx-config script name or full path
# WX_CONFIG     := wx-config
WX_CONFIG     := /usr/lib/wx/config/gtk2-ansi-release-2.6
# WX_CONFIG     := /usr/lib/wx/config/gtk2-ansi-debug-2.6
#
WX_VERSION    := $(shell echo `$(WX_CONFIG) --version`)
WX_CFLAGS     := $(shell $(WX_CONFIG) --cflags) -DUSE_WX_LIB=1
WX_CXXFLAGS   := $(shell $(WX_CONFIG) --cxxflags) -DUSE_WX_LIB=1
WX_CPPFLAGS   := $(shell $(WX_CONFIG) --cppflags) -DUSE_WX_LIB=1
WX_LIBS       := $(shell $(WX_CONFIG) --libs)
WX_GL_LIBS    := $(shell $(WX_CONFIG) --gl-libs)
WX_LDFLAGS    := $(shell $(WX_CONFIG) --ldflags 2>/dev/null)
WX_LD         := $(shell $(WX_CONFIG) --ld)
WX_PARSE_LD   := $(shell $(WX_CONFIG) --ld 2>/dev/null | sed -e 's|^[^-][^ ]*||;s| -shared | |g;s| -o *$$| |;')
WX_SOURCES    := $(shell ls wxAppMain/*.cpp Linux/wmtask*.cpp)

else
# not using wx

WX_CONFIG     :=
WX_VERSION    :=
WX_CFLAGS     := -DUSE_WX_LIB=0
WX_CXXFLAGS   := -DUSE_WX_LIB=0
WX_CPPFLAGS   := -DUSE_WX_LIB=0
WX_LIBS       :=
WX_GL_LIBS    :=
WX_LDFLAGS    :=
WX_LD         :=
WX_PARSE_LD   :=
WX_SOURCES    :=

endif
# use or not wx


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

CXXFLAGS += -g -O0
CPPFLAGS += -I/usr/include/SDL -DORBITER -DCACHE_IMAGES -DxPHONEKEYS -I$(ROOT)/Splitter -I$(ROOT)/OrbiterGen
CPPFLAGS += -DENABLE_OPENGL
LDFLAGS += -L/usr/X11R6/lib
XTRALDLIBS = -lDCECommon -lSDL -lSDL_ttf -lSDL_image -lSGE -lSDL_gfx -lXtst -lGL

CFLAGS += $(WX_CFLAGS)
CXXFLAGS += $(WX_CXXFLAGS)
CPPFLAGS += $(WX_CPPFLAGS)
XTRALDLIBS += $(WX_LIBS)

Gen_Dev_Files +=
sources += \
	$(WX_SOURCES) \
	DesignObj_Data.cpp \
	DesignObj_Orbiter.cpp \
	Simulator.cpp \
	OrbiterFileBrowser.cpp \
	Linux/OrbiterLinux.cpp \
	Linux/XRecordExtensionHandler.cpp \
	RendererMNG.cpp \
	GraphicBuilder.cpp \
	TextStyle.cpp \
	CacheImageManager.cpp \
	RendererOCG.cpp \
	ScreenHandler.cpp \
	CallBackData.cpp \
	ScreenHistory.cpp \
	MouseBehavior.cpp \
	MouseGovernor.cpp \
	MouseIterator.cpp \
	SDL/SDLRendererOCGHelper.cpp \
	SDL/SDLGraphic.cpp \
	SDL/OrbiterSDL.cpp \
	SDL/StartOrbiterSDL.cpp \
	SDL/OpenGLTextureConverter_SDL.cpp \
	OpenGL/orbitergl3dengine.cpp \
	OpenGL/Orbiter3DCommons.cpp \
	OpenGL/math3dutils.cpp \
	OpenGL/OpenGLTextureConverter.cpp \
	OpenGL/GL2DWidgets/DrawingWidgetsEngine.cpp \
	OpenGL/GL2DWidgets/basicwindow.cpp \
	OpenGL/GL2DWidgets/basewidget.cpp \
	OpenGL/GL2DWidgets/bezierwindow.cpp \
	OpenGL/GL2DEffects/gl2deffectslidefromleft.cpp \
	OpenGL/GL2DEffects/gl2deffecttransit.cpp \
	OpenGL/GL2DEffects/gl2deffectfactory.cpp \
	OpenGL/GL2DEffects/gl2deffectbeziertranzit.cpp \
	OpenGL/GL2DEffects/gl2deffectbeziertranzit_topleft.cpp \
	OpenGL/GL2DEffects/gl2deffectbeziertranzit_prism.cpp \
	OpenGL/GL2DEffects/gl2deffectbeziertranzit_flow_slideleft.cpp \
	OpenGL/GL2DEffects/gl2deffectbeziertranzit_flow_slideleft_reverse.cpp \
	OpenGL/GL2DEffects/gl2deffectfadesfromtop.cpp \
	OpenGL/GL2DEffects/gl2deffectfadesfromunderneath.cpp \
	OpenGL/GL2DEffects/GL2DEffectNoEffect.cpp \
	OpenGL/GL2DEffects/GL2DEffectBlending.cpp \
	OpenGL/GL2DEffects/GL2DEffectHighLightArea.cpp \
	OpenGL/GL2DEffects/GL2DEffectHighLight.cpp \
	OpenGL/GL2DEffects/gl2deffectselectedarea.cpp \
	OpenGL/GL2DEffects/GL2DEffectTransitionNoEffect.cpp \
	OpenGL/GL2DEffects/gl2deffect.cpp \
	Linux/OSDScreenHandler.cpp \
	Linux/MouseBehavior_Linux.cpp \
	WizardLogic.cpp \
	../CreateDevice/UserUtils.cpp \
	$(ROOT)/Splitter/TextSplitter.cpp \
	$(ROOT)/Splitter/TextWrapper.cpp \
	$(ROOT)/OrbiterGen/Renderer.cpp \
	Linux/X3DButton.cpp Linux/X3DWindow.cpp Linux/XProgressWnd.cpp Linux/XPromptUser.cpp\

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

info:
	: WX_VERSION=$(WX_VERSION)
	: ""
	: WX_CFLAGS=$(WX_CFLAGS)
	: WX_CXXFLAGS=$(WX_CXXFLAGS)
	: WX_CPPFLAGS=$(WX_CPPFLAGS)
	: ""
	: WX_LIBS=$(WX_LIBS)
	: WX_GL_LIBS=$(WX_GL_LIBS)
	: ""
	: WX_LDFLAGS=$(WX_LDFLAGS)
	: WX_LD=$(WX_LD)
	: WX_PARSE_LD=$(WX_PARSE_LD)
	: ""
	: WX_SOURCES=$(WX_SOURCES)
	: ""
	: new CFLAGS=$(CFLAGS)
	: new CXXFLAGS=$(CXXFLAGS)
	: new CPPFLAGS=$(CPPFLAGS)
	: new XTRALDLIBS=$(XTRALDLIBS)
	: ""
	: old BASELDLIBS=$(BASELDLIBS)
	: gen LDFLAGS=$(LDFLAGS)
	: gen LDLIBS=$(LDLIBS)

# -- END --
