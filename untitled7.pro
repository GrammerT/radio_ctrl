#-------------------------------------------------
#
# Project created by QtCreator 2018-01-27T13:29:01
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled7
TEMPLATE = app
RC_FILE = icon.rc

INCLUDEPATH += "$$PWD/zmq_include"

CONFIG(debug, debug|release){
    contains(QMAKE_TARGET.arch, x86){
        LIBS += $$PWD/libs/debug/x86/libzmq.lib
        message("debug x86")
    }
    contains(QMAKE_TARGET.arch, x86_64){
        LIBS += $$PWD/libs/debug/x64/libzmq.lib
        message("debug x64")
    }
}

CONFIG(release, debug|release){
    contains(QMAKE_TARGET.arch, x86){
        LIBS += $$PWD/libs/release/x86/libzmq.lib
        message("release x86")
    }
    contains(QMAKE_TARGET.arch, x86_64){
        LIBS += $$PWD/libs/release/x64/libzmq.lib
        message("release x64")
    }
}

SOURCES += main.cpp\
        widget.cpp \
    NetworkTaskWorker.cpp \
    newwidget.cpp

HEADERS  += widget.h \
    NetworkTaskWorker.h \
    newwidget.h

RESOURCES += \
    project_resource.qrc

FORMS += \
    newwidget.ui
