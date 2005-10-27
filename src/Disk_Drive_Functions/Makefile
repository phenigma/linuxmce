ROOT = ..

CXX = g++
CXXFLAGS = -W -Wall -fPIC -ggdb3
CPPFLAGS = -I. -I$(ROOT) -I$(ROOT)/DCE
LDFLAGS = -L$(ROOT)/lib

MYNAME = Disk_Drive_Functions

sources = Disk_Drive_Functions.cpp

all: $(sources:.cpp=.o)
	$(CXX) -shared -o lib$(MYNAME).so $+ $(LDFLAGS) $(LDLIBS)
	cp lib$(MYNAME).so ../lib/

clean:
	rm -f $(sources:.cpp=.o) *.so
