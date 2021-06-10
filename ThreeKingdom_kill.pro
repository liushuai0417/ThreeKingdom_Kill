#-------------------------------------------------
#
# Project created by QtCreator 2021-06-02T12:29:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ThreeKingdom_kill
TEMPLATE = app
include(./netapi/netapi.pri)
INCLUDEPATH += ./netapi/
SOURCES += main.cpp\
        dialog.cpp \
    mainscene.cpp \
    mypushbutton.cpp \
    ckernel.cpp \
    roomitem.cpp \
    chaneginfodialog.cpp \
    createroomdialog.cpp \
    addfrienddialog.cpp \
    joinroomdialog.cpp \
    friendlist.cpp \
    frienditem.cpp

HEADERS  += dialog.h \
    mainscene.h \
    mypushbutton.h \
    ckernel.h \
    roomitem.h \
    chaneginfodialog.h \
    createroomdialog.h \
    addfrienddialog.h \
    joinroomdialog.h \
    friendlist.h \
    frienditem.h

FORMS    += dialog.ui \
    mainscene.ui \
    roomitem.ui \
    chaneginfodialog.ui \
    createroomdialog.ui \
    addfrienddialog.ui \
    joinroomdialog.ui \
    friendlist.ui \
    frienditem.ui

RESOURCES += \
    resource.qrc
CONFIG+=resources_big
