ROOT=..
CXXFLAGS = -Wall -g -D_GNU_SOURCE
CPPFLAGS = -I./ -I../../ -I../ -I/usr/include/mysql -I$(ROOT) -I$(ROOT)/DCE -I$(ROOT)/BD -I$(ROOT)/Orbiter -I$(ROOT)/VIPShared -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DUSE_MYSQL -DVIPSERVER -DXBTSOCKET -DBLUETOOTH_DONGLE
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -lmysqlclient_r

sources = \
    BDCommandProcessor_BluetoothDongle.cpp \
    BD_CP_CaptureKeyboard.cpp \
    BD_CP_Disconnect.cpp \
    BD_CP_SendFile.cpp \
    BD_CP_SendMeKeystrokes.cpp \
    BD_CP_ShowImage.cpp \
    BD_CP_ShowList.cpp \
    BD_CP_ShowVMC.cpp \
    BD_CP_UpdateVariables.cpp \
    BD_PC_Disconnect.cpp \
    BD_PC_KeyWasPressed.cpp \
    BD_PC_OrderPlaced.cpp \
    BD_PC_ReportingVariables.cpp \
    BD_PC_SelectedFromList.cpp \
    BD_PC_SetVariable.cpp \
    BD_PC_VMCHidden.cpp \
    BD_PC_VMCTerminated.cpp \
    PhoneDetection_Bluetooth_Linux.cpp \
    PhoneDetectionEngine.cpp \
    PhoneDetection_Simulate.cpp \
    PlutoPhoneCommands.cpp \
    VIPIncludes.cpp
#sources = BD_CP_CaptureKeyboard.cpp  BD_CP_ShowImage.cpp  BD_CP_ShowList.cpp  BD_PC_KeyWasPressed.cpp  BD_PC_SelectedFromList.cpp  BD_PC_SetVariable.cpp

all: libVIPCommon.a all-recursive

libVIPCommon.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	cp $@ ../lib
		
clean: clean-recursive
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) libVIPCommon.a >/dev/null

include ./make.rules
-include $(sources:.cpp=.d)
