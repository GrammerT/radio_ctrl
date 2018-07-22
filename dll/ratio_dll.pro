#-------------------------------------------------
#
# Project created by QtCreator 2018-07-09T14:15:00
#
#-------------------------------------------------

QT       -= core gui

TARGET = ratio_dll
TEMPLATE = lib

DEFINES += RATIO_DLL_LIBRARY

SOURCES += ratio_dll.cpp

HEADERS += ratio_dll.h

DEF_FILE += "$$PWD/ratio_dll.def"

INCLUDEPATH += "$$PWD/zmq_include"

CONFIG(debug, debug|release){
    contains(QMAKE_TARGET.arch, x86){
        LIBS += $$PWD/libs/libzmq.lib
        message("debug x86")
    }
    contains(QMAKE_TARGET.arch, x86_64){
        LIBS += $$PWD/libs/debug/x64/libzmq.lib
        message("debug x64")
    }
}

CONFIG(release, debug|release){
    contains(QMAKE_TARGET.arch, x86){
        LIBS += $$PWD/libs/libzmq.lib
        message("release x86")
    }
    contains(QMAKE_TARGET.arch, x86_64){
        LIBS += $$PWD/libs/release/x64/libzmq.lib
        message("release x64")
    }
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

