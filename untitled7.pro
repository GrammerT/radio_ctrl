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

DEFINES += ZMQ_DLL

INCLUDEPATH += "$$PWD/zmq_include"

CONFIG(debug, debug|release){
    contains(QMAKE_TARGET.arch, x86){
        LIBS += $$PWD/libs/debug/x86/ratio_dll.lib
        LIBS += $$PWD/libs/debug/x86/libzmq.lib
        message("debug x86")
    }
}

CONFIG(release, debug|release){
    contains(QMAKE_TARGET.arch, x86){
        LIBS += $$PWD/libs/release/x86/ratio_dll.lib
        LIBS += $$PWD/libs/release/x86/libzmq.lib
        message("release x86")
    }
}

SOURCES += main.cpp\
    NetworkTaskWorker.cpp \
    newwidget.cpp

HEADERS  += \
    NetworkTaskWorker.h \
    newwidget.h

RESOURCES += \
    project_resource.qrc

FORMS += \
    newwidget.ui
