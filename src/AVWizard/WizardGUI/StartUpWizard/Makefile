ROOT = ../../..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = <-mkr_t_compile_defines-> -DDEBUG
LDFLAGS = -L$(ROOT)/lib

MYNAME = AVWizard

CXXFLAGS += -g -O0 -J2
CPPFLAGS += -I/usr/include/SDL -I/usr/include/libxml2 
LDLIBS = -lSDL -lSDL_ttf -lSDL_image -lxml2 -lxine -lSDL_gfx -lGL -lGLU -lXrender 
FLAGS = $(CXXFLAGS) $(CPPFLAGS)
Gen_Dev_Files +=
include sources.list.mk
    
AVWizard: $(sources:.cpp=.o)
	$(CXX) -o AVWizard $+ $(FLAGS) $(LDFLAGS) $(LDLIBS)
	cp AVWizard ../../../bin/

clean: 
	rm -f *.o *.d AVWizard 

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
