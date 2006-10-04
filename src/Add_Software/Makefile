ROOT = ..

bin_PROGRAMS = getxmls instcheck

CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
# the library search path.
getxmls_LDADD = -lmysqlclient -ldancer-xml
instcheck_LDADD = -lmysqlclient

MYNAME_getxmls = getxmls
VERSION_getxmls = 0.0.1

MYNAME_instcheck = instcheck
VERSION_instcheck = 0.0.1

getxmls_SOURCES = getxmls.cpp ../PlutoUtils/uuencode.cpp
instcheck_SOURCES = instcheck.cpp

all: bin

whattodo:
	@echo "type 'make bin', 'make getxmls', 'make instcheck', 'make all'"

bin: getxmls instcheck

getxmls: $(getxmls_SOURCES:.cpp=.o)
	$(CXX) -o $(MYNAME_getxmls) $(call cpath,$+) $(getxmls_LDADD)
	cp $@ ../bin

instcheck: $(instcheck_SOURCES:.cpp=.o)
	$(CXX) -o $(MYNAME_instcheck) $(instcheck_SOURCES) $(instcheck_LDADD)
	cp $@ ../bin

clean:
	rm -f $(bin_PROGRAMS)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
