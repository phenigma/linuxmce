#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -DDEBUG
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lpthread -lDCECommon -lmysqlclient_r

MYNAME = Orbiter
VERSION = 2.0.0
MYDEVTEMPLATEID = 8mplateID

Gen_Dev_Files = $(GENDEV)/OrbiterBase.cpp
sources = Main.cpp Orbiter.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/SDL -DORBITER -DCACHE_IMAGES -DDEBUG -DxTHREAD_LOG -I$(ROOT)/Splitter -I$(ROOT)/OrbiterGen
LDFLAGS +=
LDLIBS += -lPlutoUtils -lSerializeClass -lDCECommon -lpluto_main -lSDL -lSDL_ttf -lSDL_image -lSGE

Gen_Dev_Files +=
sources +=  DesignObj_Data.cpp DesignObj_Orbiter.cpp \
			Linux/OrbiterLinux.cpp \
			SDL/OrbiterSDL.cpp SDL/StartOrbiterSDL.cpp \
			$(ROOT)/Splitter/TextSplitter.cpp \
			$(ROOT)/Splitter/TextWrapper.cpp \
			$(ROOT)/OrbiterGen/Renderer.cpp

#<-dceag-mf-b->
whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Orbiter

so: Orbiter.so

Orbiter.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Orbiter: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) Orbiter.bin Orbiter.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/DCE/make.rules
-include $(sources:.cpp=.d)
#<-dceag-mf-e->

