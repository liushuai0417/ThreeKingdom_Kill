#-------------------------------------------------
#
# Project created by QtCreator 2021-06-02T12:29:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ThreeKingdom_kill
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    mainscene.cpp

HEADERS  += dialog.h \
    packdef.h \
    mainscene.h

FORMS    += dialog.ui \
    mainscene.ui

RESOURCES += \
    resource.qrc
CONFIG+=resources_big
