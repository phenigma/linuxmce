#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -DDEBUG
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

MYNAME = Bluetooth_Dongle
VERSION = 2.0.0
MYDEVTEMPLATEID = 13mplateID

Gen_Dev_Files = $(GENDEV)/Bluetooth_DongleBase.cpp
sources = Main.cpp Bluetooth_Dongle.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/SDL -DORBITER -DBLUETOOTH_DONGLE
LDFLAGS +=
XTRALDLIBS = -lVIPCommon -lBDCommon -lbluetooth -lpluto_main -lSDL -lSDL_ttf -lSDL_image -lSGE

Gen_Dev_Files += $(GENDEV)/OrbiterBase.cpp
sources += \
	../Orbiter/SDL_Bluetooth/OrbiterSDLBluetooth.cpp \
	../Orbiter/SDL_Bluetooth/StartOrbiterSDLBluetooth.cpp \
	../Orbiter/DesignObj_Data.cpp \
	../Orbiter/DesignObj_Orbiter.cpp \
	../Orbiter/Orbiter.cpp \
	../Orbiter/SDL/OrbiterSDL.cpp \
	../Splitter/TextWrapper.cpp \
	../Splitter/TextSplitter.cpp \
	../OrbiterGen/Renderer.cpp \
	../VIPShared/PlutoPhoneCommands.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Bluetooth_Dongle

so: Bluetooth_Dongle.so

Bluetooth_Dongle.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Bluetooth_Dongle: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) Bluetooth_Dongle.bin Bluetooth_Dongle.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
#<-dceag-mf-e->

