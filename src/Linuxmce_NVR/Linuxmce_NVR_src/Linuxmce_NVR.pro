#-------------------------------------------------
#
# Project created by QtCreator 2015-07-25T13:46:02
#
#-------------------------------------------------

QT       += core network gui

TARGET = ../Linuxmce_NVR
CONFIG   += console c++11
CONFIG   -= app_bundle

LIBS += -lqhttpserver

QMAKE_CXXFLAGS += -DUSE_LZO_DATAGRID
#LinuxMCE Specific include path. Linking in the app instead of against dce libs for multi-platform expediency.
INCLUDEPATH += ../../ ../../DCE/

TEMPLATE = app

HEADERS += ../LinuxMCE_NVR.h \
        ../../Gen_Devices/Linuxmce_NVRBase.h \
        ../../PlutoUtils/ThreadedClass.h \
        ../../PlutoUtils/MultiThreadIncludes.h \
        ../../PlutoUtils/StringUtils.h \
        ../../PlutoUtils/CommonIncludes.h \
        ../../PlutoUtils/Other.h \
        ../../PlutoUtils/getch.h \
        ../../PlutoUtils/MyStl.h \
        ../../DCE/DeviceData_Base.h \
        ../../DCE/Message.h \
        ../../DCE/ServerLogger.h \
        ../../DCE/Logger.h \
        ../../DCE/Virtual_Device_Translator.h \
        ../../DCE/PlutoLockLogger.h \
        ../../DCE/ClientSocket.h \
        ../../DCE/PlainClientSocket.h \
        ../../DCE/AlarmManager.h \
        ../../SerializeClass/SerializeClass.h \
        ../../PlutoUtils/FileUtils.h \
        ../../pluto_main/Define_DeviceCategory.h \
        ../../pluto_main/Define_DeviceTemplate.h \
    ../cameraClasses/abstractcameraevent.h \
    ../cameraClasses/abstractpicamera.h \
    ../cameraClasses/motionpicamera.h \
    ../managerClasses/eventlistener.h \
    ../managerClasses/httpresponder.h \
    ../managerClasses/NvrManager.h

SOURCES += ../Main.cpp \
           ../Linuxmce_NVR.cpp \
        ../../Gen_Devices/Linuxmce_NVRBase.cpp \
        ../../PlutoUtils/uuencode.cpp \
        ../../PlutoUtils/ThreadedClass.cpp \
        ../../PlutoUtils/Other.cpp \
        ../../PlutoUtils/MultiThreadIncludes.cpp \
        ../../PlutoUtils/minilzo.cpp \
        ../../PlutoUtils/md5c.cpp \
        ../../PlutoUtils/FileUtils.cpp \
        ../../PlutoUtils/CommonIncludes.cpp \
        ../../SerializeClass/SerializeClass.cpp \
        ../../DCE/Virtual_Device_Translator.cpp \
        ../../DCE/Socket.cpp \
        ../../DCE/ServerLogger.cpp \
        ../../DCE/PlainClientSocket.cpp \
        ../../DCE/MessageBuffer.cpp \
        ../../DCE/Message.cpp \
        ../../DCE/HandleRequestSocket.cpp \
        ../../DCE/Logger.cpp \
        ../../DCE/Event_Impl.cpp \
        ../../DCE/DCEConfig.cpp \
        ../../DCE/DataGrid.cpp \
        ../../DCE/Command_Impl.cpp \
        ../../DCE/AlarmManager.cpp \
        ../../PlutoUtils/StringUtils.cpp \
        ../../DCE/ClientSocket.cpp \
        ../../DCE/DeviceData_Base.cpp \
        ../../DCE/DeviceData_Impl.cpp \
        ../../PlutoUtils/getch.cpp \
    ../cameraClasses/abstractcameraevent.cpp \
    ../cameraClasses/abstractpicamera.cpp \
    ../cameraClasses/motionpicamera.cpp \
    ../managerClasses/eventlistener.cpp \
    ../managerClasses/httpresponder.cpp \
    ../managerClasses/NvrManager.cpp
