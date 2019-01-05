QT += core
QT -= gui

CONFIG += c++11

TARGET = qt_cmd_server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += "$$PWD/../zmq_include"


SOURCES += main.cpp \
    zmq_thread.cpp

HEADERS += \
    zmq_thread.h \


LIBS += $$PWD/../libs/release/x86/libzmq.lib


