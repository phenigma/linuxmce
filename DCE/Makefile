VIP = ../VIPShared
GDEV = ./Gen_Devices
CXXFLAGS = -Wall -ggdb3 
CPPFLAGS = -I./ -I../ -I../DCE/ -I../VIPShared/ -I/usr/include/mysql -DXLL_DEBUG_FILE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DxTHREAD_LOG -DxDEBUG -DUSE_LZO_DATAGRID -D_GNU_SOURCE
CXX = g++

APPNAME = DCECommon
VERSION = 2.0.0

sources = minilzo.cpp DataGrid.cpp DCEClientSocket.cpp DCELogger.cpp DCEMessage.cpp \
	DCESocket.cpp DCEServerSocket.cpp DCESocketListener.cpp DCEServerLogger.cpp \
	AlarmManager.cpp md5c.cpp DCECommand_Impl.cpp DCEEvent_Impl.cpp \
	DCEHandleRequestSocket.cpp TokenStream.cpp CommonIncludes.cpp \
	DCEDeviceData_Impl.cpp DCEDeviceData_Base.cpp \
	MultiThreadIncludes.cpp

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

include ./make.rules
-include $(sources:.cpp=.d)
