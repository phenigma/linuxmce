ROOT = ..

bin_PROGRAMS = Tribunexml headendtobase rmprogs

CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lmysqlclient_r -lSerializeClass -lDCECommon -lPlutoUtils

LDLIBS = $(BASELDLIBS)

MYNAME = Tribunexml
MYNAME2 = headendtobase
VERSION = 0.0.1

sources = Tribunexml.cpp
sources2 =  headendtobase.cpp

MYNAME3 = rmprogs

sources3 = rmprogs.cpp

whattodo:
	@echo "type 'make bin' or 'make all'"

all: bin

bin: Tribunexml headendtobase rmprogs

Tribunexml: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin

headendtobase: $(sources2:.cpp=.o)
	$(CXX) -o $(MYNAME2) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin

rmprogs: $(sources3:.cpp=.o)
	$(CXX) -o $(MYNAME3) $(call cpath,$+) $(LDFLAGS)
	cp $@ ../bin

clean:
	rm -f $(sources:.cpp=.o) $(sources2:.cpp=.o) $(sources3:.cpp=.o)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
