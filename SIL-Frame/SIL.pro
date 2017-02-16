#-------------------------------------------------
#
# Project created by QtCreator 2016-05-30T17:16:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SIL-Frame
TEMPLATE = app
CONFIG += debug_and_release


SOURCES += main.cpp\
    mainwindow.cpp \
    dialognew.cpp \
    dialogadd.cpp \
    dialogremove.cpp \
    dialogsettings.cpp \
    dialogaddplug.cpp \
    dialogremoveplug.cpp \
    dialogdepsil.cpp \
    dialogduppro.cpp \
    sshhandler.cpp \
    dialogpass.cpp \
    sshhelper.cpp \
    pluginhandler.cpp \
    dialogdelete.cpp \
    deletehandler.cpp \
    addplughandler.cpp \
    newhandler.cpp \
    passwhandler.cpp

HEADERS  += \
    mainwindow.h \
    dialognew.h \
    dialogadd.h \
    dialogremove.h \
    dialogsettings.h \
    dialogaddplug.h \
    dialogremoveplug.h \
    dialogdepsil.h \
    dialogduppro.h \
    sshhandler.h \
    dialogpass.h \
    sshhelper.h \
    pluginhandler.h \
    dialogdelete.h \
    deletehandler.h \
    addplughandler.h \
    newhandler.h \
    passwhandler.h

FORMS    += mainwindow.ui \
    dialognew.ui \
    dialogadd.ui \
    dialogremove.ui \
    dialogsettings.ui \
    dialogaddplug.ui \
    dialogremoveplug.ui \
    dialogdepsil.ui \
    dialogduppro.ui \
    dialogpass.ui \
    dialogdelete.ui

RESOURCES += \
    resources.qrc


