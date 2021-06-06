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
    chaneginfodialog.cpp

HEADERS  += dialog.h \
    packdef.h \
    mainscene.h \
    mypushbutton.h \
    ckernel.h \
    roomitem.h \
    chaneginfodialog.h

FORMS    += dialog.ui \
    mainscene.ui \
    roomitem.ui \
    chaneginfodialog.ui

RESOURCES += \
    resource.qrc
CONFIG+=resources_big
