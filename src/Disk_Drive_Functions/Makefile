ROOT = ..
SNR_CPPFLAGS ?= <-mkr_t_compile_defines->

CXX = g++
CXXFLAGS = -W -Wall -fPIC -ggdb3
CPPFLAGS = -I. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql $(SNR_CPPFLAGS)
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread -lpluto_media 
MYNAME = Disk_Drive_Functions

sources = Disk_Drive_Functions.cpp RipTask.cpp RipJob.cpp IdentifyJob.cpp IdentifyTask.cpp MoveDiscTask.cpp JukeBox.cpp ../Media_Plugin/MediaAttributes_LowLevel.cpp 

all: $(sources:.cpp=.o)
	$(CXX) -shared -o lib$(MYNAME).so $+ $(LDFLAGS) $(LDLIBS)
	cp lib$(MYNAME).so ../lib/

clean:
	rm -f $(sources:.cpp=.o) *.so
