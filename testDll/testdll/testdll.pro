TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

#INCLUDEPATH += "$$PWD/../../dll"

#DEPENDPATH  += "$$PWD/../../dll"

#CONFIG(debug, debug|release){
#    contains(QMAKE_TARGET.arch, x86){
#        LIBS += $$PWD/../ratio_dll/release/ratio_dll.lib
#    }
#}

