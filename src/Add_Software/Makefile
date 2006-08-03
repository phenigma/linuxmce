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

whattodo:
	@echo "type 'make bin', 'make getxmls', 'make instcheck', 'make all'"

all: bin

bin: getxmls instcheck

getxmls:
	$(CXX) -o $(MYNAME_getxmls) $(getxmls_SOURCES) $(getxmls_LDADD)
	cp $@ ../bin

instcheck:
	$(CXX) -o $(MYNAME_instcheck) $(instcheck_SOURCES) $(instcheck_LDADD)
	cp $@ ../bin

clean:
	rm -f $(bin_PROGRAMS)
