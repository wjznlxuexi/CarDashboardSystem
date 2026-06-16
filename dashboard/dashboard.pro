QT += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = dashboard
TEMPLATE = app

SOURCES += main.cpp \
           dashboardwindow.cpp \
           network/networkclient.cpp \
           gauges/speedometerwidget.cpp \
           gauges/rpmgaugewidget.cpp \
           gauges/fuelgaugewidget.cpp \
           gauges/tempgaugewidget.cpp \
           indicators/turnindicatorwidget.cpp \
           indicators/warninglightswidget.cpp \
           audio/warningsoundsystem.cpp

HEADERS += dashboardwindow.h \
           common/vehicledata.h \
           common/warningtype.h \
           network/networkclient.h \
           gauges/speedometerwidget.h \
           gauges/rpmgaugewidget.h \
           gauges/fuelgaugewidget.h \
           gauges/tempgaugewidget.h \
           indicators/turnindicatorwidget.h \
           indicators/warninglightswidget.h \
           audio/warningsoundsystem.h

FORMS += dashboardwindow.ui

RESOURCES += resources/dashboard.qrc \
    resources/dashboard.qrc

INCLUDEPATH += $$PWD \
               $$PWD/common \
               $$PWD/network \
               $$PWD/gauges \
               $$PWD/indicators \
               $$PWD/audio
