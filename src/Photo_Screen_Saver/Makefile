#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= <-mkr_t_compile_defines->

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -I../STEngine <-mkr_t_compile_defines->
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

MYNAME = Photo_Screen_Saver
VERSION = 2.0.0
MYDEVTEMPLATEID = 1825mplateID

Gen_Dev_Files = $(GENDEV)/Photo_Screen_SaverBase.cpp
sources = Main.cpp Photo_Screen_Saver.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/SDL
LDFLAGS += -lGL -lGLU -lSDL -lSDL_image -lSDL_ttf -lXrender
XTRALDLIBS =

Gen_Dev_Files +=
sources += AnimatedPicture.cpp \
	FadeBase.cpp \
	FadeBlending.cpp \
	FadeFactory.cpp \
	FileBrowser.cpp \
	GaleryStateMachine.cpp \
	Gallery.cpp \
	GalleryScenario.cpp \
	GUIWizardUtils.cpp \
	../STEngine/ExtensionManager.cpp \
	../STEngine/ColorDesc.cpp \
	../STEngine/GraphicImage.cpp \
	../STEngine/MathUtils.cpp \
	../STEngine/Builder.cpp \
	../STEngine/Container.cpp \
	../STEngine/Frame.cpp \
	../STEngine/Painter.cpp \
	../STEngine/Transform.cpp \
	../STEngine/Triangle.cpp \
	../STEngine/Vertex.cpp \
	../STEngine/Point3D.cpp \
	../STEngine/TextureManager.cpp \
	RefreshRates.cpp \
	SDLFrontEnd.cpp \
	WM_Event.cpp \
	ZoomBase.cpp \
	ZoomFactory.cpp \
	ZoomPanUp.cpp \
	ZoomSimple.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Photo_Screen_Saver

so: Photo_Screen_Saver.so

Photo_Screen_Saver.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Photo_Screen_Saver: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Photo_Screen_Saver.bin Photo_Screen_Saver.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

