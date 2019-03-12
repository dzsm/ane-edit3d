#-------------------------------------------------
#
# Project created by QtCreator 2015-09-24T16:28:24
#
#-------------------------------------------------

QT       += core gui opengl #script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Edit3D
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    glview/glview.cpp \
    control/control.cpp \
    core/element.cpp \
    core/molecule.cpp \
    glview/geometries.cpp \
    glview/camera.cpp \
    control/moleculedata.cpp \
    command/commandcontrol.cpp \
    slider/frameslider.cpp \
    command/actionrun.cpp \
    statusbar/statusbar.cpp

HEADERS  += mainwindow.h \
    glview/glview.h \
    control/control.h \
    core/core.h \
    core/element.h \
    core/framenavigation.h \
    core/molecule.h \
    core/neighbourlist.h \
    glview/camera.h \
    glview/geometries.h \
    control/moleculedata.h \
    command/commandcontrol.h \
    slider/frameslider.h \
    command/actionrun.h \
    statusbar/statusbar.h

FORMS    += mainwindow.ui
//
QMAKE_CXXFLAGS += -O3

CONFIG += c++11
INCLUDEPATH += /usr/include/eigen3
INCLUDEPATH += .

RESOURCES += \
    shaders.qrc \
    images.qrc \
    scripts.qrc

# for window icon
RC_FILE = app.rc
# for mac icons directly
# ICON = myapp.icns

DISTFILES +=
