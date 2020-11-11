#include "mainwindow.h"
#include <QApplication>

QString _defaultPathAudio = "";
QString _defaultPathProjects = "";
QString _defaultPathOutput = "";

int main(int argc, char *argv[])
{
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
