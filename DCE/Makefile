ROOT = ..
VIP = ../VIPShared
GDEV = ./Gen_Devices
CXXFLAGS = -Wall -ggdb3
CPPFLAGS = -I./ -I../ -I../DCE/ -I../VIPShared/ -I/usr/include/mysql -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_DEVEL_DEFINES -DUSE_LZO_DATAGRID -D_GNU_SOURCE
CXX = g++

APPNAME = DCECommon
VERSION = 2.0.0

sources = DataGrid.cpp ClientSocket.cpp Logger.cpp Message.cpp \
	Socket.cpp ServerSocket.cpp SocketListener.cpp ServerLogger.cpp \
	AlarmManager.cpp Command_Impl.cpp Event_Impl.cpp \
	HandleRequestSocket.cpp \
	DeviceData_Impl.cpp DeviceData_Base.cpp \
	DCEConfig.cpp DCEMySqlConfig.cpp

all: libDCECommon all-recursive

libDCECommon: libDCECommon.so libDCECommon.a

libDCECommon.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $+
#	cp $@ ../lib

libDCECommon.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	cp $@ ../lib

clean:
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) libDCECommon.so libDCECommon.a >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
