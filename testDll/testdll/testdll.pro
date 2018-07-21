TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c

INCLUDEPATH += "$$PWD/../ratio_dll1"

DEPENDPATH  += "$$PWD/../ratio_dll1"

#CONFIG(debug, debug|release){
#    contains(QMAKE_TARGET.arch, x86){
#        LIBS += $$PWD/../ratio_dll/release/ratio_dll.lib
#    }
#}

