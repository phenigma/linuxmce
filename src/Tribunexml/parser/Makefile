ROOT = ../..

bin_PROGRAMS = Tribunexml headendtobase

CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib

MYNAME = rmprogs
VERSION = 0.0.1

sources = rmprogs.cpp

whattodo:
	@echo "type 'make bin', 'make webinst', 'make instcheck', 'make all'"

all: bin

bin: rmprogs

rmprogs: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
