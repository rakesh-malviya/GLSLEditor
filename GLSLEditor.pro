#-------------------------------------------------
#
# Project created by QtCreator 2014-06-12T21:15:31
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = DistanceField
TEMPLATE = app
LIBS += -lfreeglut


SOURCES += main.cpp\
        mainwindow.cpp \
    qglframe.cpp \
    qglrenderthread.cpp \
    glsleditor.cpp \
    highlighter.cpp \
    constantfinder.cpp

HEADERS  += mainwindow.h \
    qglframe.h \
    qglrenderthread.h \
    glsleditor.h \
    highlighter.h \
    constantfinder.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    Basic.vsh \
    Basic.fsh
