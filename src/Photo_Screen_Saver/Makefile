#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
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
	ColorDesc.cpp \
	ExtensionManager.cpp \
	FadeBase.cpp \
	FadeBlending.cpp \
	FadeFactory.cpp \
	FileBrowser.cpp \
	GaleryStateMachine.cpp \
	Gallery.cpp \
	GalleryScenario.cpp \
	GraphicImage.cpp \
	GUIWizardUtils.cpp \
	MathUtils.cpp \
	MeshBuilder.cpp \
	MeshContainer.cpp \
	MeshFrame.cpp \
	MeshPainter.cpp \
	MeshTransform.cpp \
	MeshTriangle.cpp \
	MeshVertex.cpp \
	Point3D.cpp \
	RefreshRates.cpp \
	SDLFrontEnd.cpp \
	TextureManager.cpp \
	WM_Event.cpp \
	ZoomBase.cpp \
	ZoomFactory.cpp \
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

