#include "mainwindow.h"
#include <QApplication>

QString _defaultPathAudio = "";
QString _defaultPathProjects = "";
QString _defaultPathOutput = "";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
