#-------------------------------------------------
#
# Project created by QtCreator 2015-06-23T22:05:53
#
#-------------------------------------------------

QT       += network core

QT       -= gui

TARGET = googlApiInterface
TEMPLATE = lib
CONFIG += plugin
DESTDIR=../../bin

DEFINES += GOOGLAPIINTERFACE_LIBRARY

SOURCES += googleapiplugin.cpp

HEADERS += googleapiplugin.h\
        googlapiinterface_global.h

unix {
    target.path =../../bin
    INSTALLS += target
}

DISTFILES += \
    googleapi.json
