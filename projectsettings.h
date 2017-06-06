#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVector>

class ProjectSettings
{
public:
    void loadProjectSettings(const QString projectFileName);
    void saveProjectSettings(const QString projectFileName);

    const int currentVersion = 130;
    const QString ModeName_Main = "Main";
    const QString ModeName_Grid = "Grid";
    const QString ModeName_Steps = "Steps";
    const QString ModeName_Megabreak = "Megabreak";

    int version = 0;
    QString modeName = ModeName_Main;
    int sampleCount = 0;
    int channelCount = 2;
    int sampleRate = 44100;
    int bitRate = 16;
    int loopSetting = 0;
    int stretchSetting = 0;
    int trigQuantSetting = 0;
    int gain = 0;
    int tempo = 500;
    int normalizationMode = 0;
    int sliceSteps = 0;
    QVector<QString> samplePaths;

private:
    QString findSavedProjectLine(QTextStream &stream, QString searchText);

};

#endif // PROJECTSETTINGS_H
