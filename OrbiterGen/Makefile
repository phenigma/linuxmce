TS = ../Splitter
ORB = ../Orbiter

ROOT = ..
CXXFLAGS = -Wall -ggdb3
CPPFLAGS = -I/usr/local/include -I$(ROOT)/DCE -I.. -I. -I/usr/include/mysql -I/usr/include/SDL -I$(TS) -DOrbiterGen
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -ldl -lpluto_main -lDCECommon -lPlutoUtils -lSerializeClass -lSDL -lSGE -lSDL_image -lSDL_ttf -lmysqlclient_r

APPNAME = OrbiterGen
VERSION = 2.0.0

sources = \
	OrbiterGen.cpp \
	OGCommand.cpp \
	OGArray.cpp \
	DesignObj_Generator.cpp \
	OGText.cpp Renderer.cpp \
	$(TS)/TextSplitter.cpp \
	$(TS)/TextWrapper.cpp \
	$(ORB)/DesignObj_Data.cpp \
	$(ORB)/TextStyle.cpp \
	$(ORB)/RendererMNG.cpp

all: $(sources:.cpp=.o)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $(APPNAME) $+ $(LDFLAGS) $(LDLIBS)
	cp OrbiterGen ../bin
	
clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) $(APPNAME) >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
