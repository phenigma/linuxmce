ROOT=..
CXXFLAGS = -Wall -g -D_GNU_SOURCE
CPPFLAGS = -I./ -I../../ -I../ -I/usr/include/mysql -I$(ROOT) -I$(ROOT)/DCE -I$(ROOT)/BD -I$(ROOT)/Orbiter -I$(ROOT)/VIPShared -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DUSE_MYSQL -DVIPSERVER -DXBTSOCKET -DBLUETOOTH_DONGLE
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -lmysqlclient_r

sources = BD_CP_CaptureKeyboard.cpp  BD_CP_ShowImage.cpp  BD_CP_ShowList.cpp  BD_PC_KeyWasPressed.cpp  BD_PC_SelectedFromList.cpp  BD_PC_SetVariable.cpp

all: libBDSymbian.a all-recursive

libBDSymbian.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	cp $@ ../lib
		
clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) libBDSymbian.a >/dev/null

include ./make.rules
-include $(sources:.cpp=.d)
