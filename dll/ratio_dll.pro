#-------------------------------------------------
#
# Project created by QtCreator 2018-07-09T14:15:00
#
#-------------------------------------------------

QT       -= core

TARGET = ratio_dll
TEMPLATE = lib

DEFINES += RATIO_DLL_LIBRARY



SOURCES += ratio_dll.cpp

HEADERS += ratio_dll.h

DEF_FILE += "$$PWD/ratio_dll.def"

INCLUDEPATH += "$$PWD/zmq_include"

CONFIG(debug, debug|release){
QMAKE_CFLAGS_DEBUG += -MDd
QMAKE_CXXFLAGS_DEBUG += -MDd
    contains(QMAKE_TARGET.arch, x86){
        LIBS += $$PWD/libs/libzmq.lib
        message("debug x86")
    }
}

CONFIG(release, debug|release){
QMAKE_CFLAGS_RELEASE += -MD
QMAKE_CXXFLAGS_RELEASE += -MD
    contains(QMAKE_TARGET.arch, x86){
        LIBS += $$PWD/libs/libzmq.lib
        message("release x86")
    }
}


