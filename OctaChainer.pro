#-------------------------------------------------
#
# Project created by QtCreator 2016-06-08T19:43:22
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OctaChainerv1_3_OSX
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    audiofactory.cpp \
    otwriter.cpp \
    tabmainwidget.cpp \
    tabmegabreakwidget.cpp \
    tabgridwidget.cpp \
    tabstepswidget.cpp

HEADERS  += mainwindow.h \
    audiofactory.h \
    otwriter.h \
    tabmainwidget.h \
    tabmegabreakwidget.h \
    tabgridwidget.h \
    tabstepswidget.h

FORMS    += mainwindow.ui \
    tabmainwidget.ui \
    tabmegabreakwidget.ui \
    tabgridwidget.ui \
    tabstepswidget.ui

LIBS += -L$$PWD/libsndfile/lib/ -lsndfile.1
INCLUDEPATH += $$PWD/libsndfile/include
DEPENDPATH += $$PWD/libsndfile/include
