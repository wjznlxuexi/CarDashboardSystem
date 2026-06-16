QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = final_simulator
TEMPLATE = app

SOURCES += main.cpp \
           simulatorwindow.cpp \
           simulatorserver.cpp \
           signalgenerator.cpp \
           faultgenerator.cpp

HEADERS += simulatorwindow.h \
           simulatorserver.h \
           signalgenerator.h \
           faultgenerator.h \
           common/vehicledata.h

FORMS += simulatorwindow.ui

INCLUDEPATH += $$PWD \
               $$PWD/common
