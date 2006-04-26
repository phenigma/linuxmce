ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib

MYNAME = AVWizard

CXXFLAGS += -g -O0
CPPFLAGS += -I/usr/include/SDL -I/usr/include/libxml2 
LDLIBS = -lSDL -lSDL_ttf -lSDL_image -lxml2
FLAGS = $(CXXFLAGS) $(CPPFLAGS)
Gen_Dev_Files +=
include sources.list.mk
    

whattodo:
	@echo "type 'make bin', 'make all' or 'make clean'"

all: bin 
bin: AVWizard

AVWizard: $(sources:.cpp=.o)
	$(CXX) -o AVWizard $(sources)  $(FLAGS) $(LDFLAGS) $(LDLIBS)

clean: 
	rm -f *.o *.d AVWizard 