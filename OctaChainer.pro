#-------------------------------------------------
#
# Project created by QtCreator 2016-06-08T19:43:22
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

macx{
    TARGET = OctaChainerv1_3_OSX
}
else
{
    TARGET = OctaChainer
}

TEMPLATE = app


SOURCES += main.cpp\
    audioutil.cpp \
    mainwindow.cpp \
    audiofactory.cpp \
    otwriter.cpp \
    tabmainwidget.cpp \
    tabmegabreakwidget.cpp \
    tabgridwidget.cpp \
    tabstepswidget.cpp \
    projectsettings.cpp

HEADERS  += mainwindow.h \
    audiofactory.h \
    audioutil.h \
    otwriter.h \
    tabmainwidget.h \
    tabmegabreakwidget.h \
    tabgridwidget.h \
    tabstepswidget.h \
    projectsettings.h

FORMS    += mainwindow.ui \
    tabmainwidget.ui \
    tabmegabreakwidget.ui \
    tabgridwidget.ui \
    tabstepswidget.ui

macx{
    LIBS += -L$$PWD/libsndfile/lib/ -lsndfile.1
    INCLUDEPATH += $$PWD/libsndfile/include
    DEPENDPATH += $$PWD/libsndfile/include
}else{
    *-g++{
        LIBS+=-lsndfile
    }
}
