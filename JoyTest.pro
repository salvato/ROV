#-------------------------------------------------
#
# Project created by QtCreator 2015-09-30T11:01:26
#
#-------------------------------------------------

TARGET = JoyTest
TEMPLATE = app
CONFIG 	   += c++11

QT       += core
QT       += gui
QT       += multimedia
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


SOURCES += main.cpp\
    mainwindow.cpp \
    geometryengine.cpp \
    glwidget.cpp \
    GrCamera.cpp \
    shimmer3box.cpp

HEADERS  += mainwindow.h \
    geometryengine.h \
    glwidget.h \
    GrCamera.h \
    shimmer3box.h

RESOURCES += \
    shaders.qrc \
    textures.qrc \
    otherresources.qrc


# Edit below for custom library location

DISTFILES +=
