ROOT = ..

bin_PROGRAMS = Tribunexml headendtobase

CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lmysqlclient_r -lPlutoUtils -lDCECommon -lSerializeClass

LDLIBS = $(BASELDLIBS)

MYNAME = Tribunexml
VERSION = 0.0.1

sources = Tribunexml.cpp
sources2 =  headendtobase.cpp

whattodo:
	@echo "type 'make bin', 'make webinst', 'make instcheck', 'make all'"

all: bin

bin: Tribunexml headendtobase

Tribunexml: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin

headendtobase: $(sources2:.cpp=.o)
	$(CXX) -o $(MYNAME2) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
