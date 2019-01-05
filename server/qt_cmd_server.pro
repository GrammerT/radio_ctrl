QT += core
QT -= gui

CONFIG += c++11

TARGET = qt_cmd_server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += "$$PWD/../radio_ctrl/zmq_include"


SOURCES += main.cpp \
    zmq_thread.cpp

HEADERS += \
    zmq_thread.h \


LIBS += $$PWD/../radio_ctrl/libs/release/x86/libzmq.lib


