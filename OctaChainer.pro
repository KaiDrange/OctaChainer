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
    sliceListItem.cpp \
    tabmainwidget.cpp \
    tabmegabreakwidget.cpp \
    tabgridwidget.cpp \
    tabstepswidget.cpp \
    projectsettings.cpp

HEADERS  += mainwindow.h \
    audiofactory.h \
    audioutil.h \
    libsndfile/include/_oldsndfile.h \
    libsndfile/include/_oldsndfile.hh \
    libsndfile/includeOSX/sndfile.h \
    libsndfile/includeOSX/sndfile.hh \
    libsndfile/macx/include/sndfile.h \
    libsndfile/macx/include/sndfile.hh \
    otwriter.h \
    sliceListItem.h \
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

macx {
    LIBS += -L$$PWD/libsndfile/macx/lib/ -lsndfile.1
    INCLUDEPATH += $$PWD/libsndfile/macx/include
    DEPENDPATH += $$PWD/libsndfile/macx/include
    ICON = $$PWD/icons/octachainer-logo.icns
}
win32 {
    LIBS += -L$$PWD/libsndfile/lib/ -llibsndfile-1
    INCLUDEPATH += $$PWD/libsndfile/include
    DEPENDPATH += $$PWD/libsndfile/include
    RC_ICONS += octachainer_logo.ico
}

linux {
    *-g++*{
        LIBS+=-lsndfile
    }
}

DISTFILES += \
    icons/octachainer-logo.icns \
    libsndfile/lib/libsndfile.1.dylib \
    libsndfile/lib/pkgconfig/sndfile.pc \
    libsndfile/macx/lib/libsndfile.1.dylib
