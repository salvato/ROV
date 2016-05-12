#-------------------------------------------------
#
# Project created by QtCreator 2015-09-30T11:01:26
#
#-------------------------------------------------

QT       += core
QT       += gui
QT       += multimedia
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JoyTest
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    joystick.cpp \
    joystickevent.cpp \
    geometryengine.cpp \
    glwidget.cpp \
    GrCamera.cpp \
    shimmer3box.cpp

HEADERS  += mainwindow.h \
    joystick.h \
    joystickevent.h \
    geometryengine.h \
    glwidget.h \
    GrCamera.h \
    shimmer3box.h

RESOURCES += \
    shaders.qrc \
    textures.qrc
