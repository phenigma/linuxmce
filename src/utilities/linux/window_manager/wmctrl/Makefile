CPPFLAGS += -g -O2 -W -Wall -Wunused -Wno-system-headers
CPPFLAGS += -I/usr/X11R6/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
DEFS     += -DPACKAGE=\"wmctrl\"
LIBS     += -L/usr/X11R6/lib -lX11 -lXmu -lglib-2.0

PROGRAMS = wmctrl
HEADERS = wmctrl.h ../../wrapper/wrapper_x11.h
SOURCES = main.cpp wmctrl.cpp ../../wrapper/wrapper_x11.cpp
OBJECTS = main.o wmctrl.o ../../wrapper/wrapper_x11.cpp

all: $(PROGRAMS)

.SUFFIXES:
.SUFFIXES: .cpp .o

clean: 
	@rm -f $(PROGRAMS) *.d *.o core *.core

.cpp.o: $(HEADERS)
	$(CXX) $(DEFS) $(CPPFLAGS) -MT $@ -MD -MP -c -o $@ $<

$(PROGRAMS): $(OBJECTS) $(HEADERS)
	@rm -f $(PROGRAMS)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)
