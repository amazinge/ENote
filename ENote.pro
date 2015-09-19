#-------------------------------------------------
#
# Project created by QtCreator 2015-08-01T17:39:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ENote
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    flatbutton.cpp \
    noteedit.cpp \
    aboutdialog.cpp \
    function.cpp

HEADERS  += widget.h \
    flatbutton.h \
    noteedit.h \
    stable.h \
    aboutdialog.h \
    function.h

FORMS    += widget.ui \
    aboutdialog.ui

RESOURCES += \
    rc.qrc

PRECOMPILED_HEADER = stable.h

