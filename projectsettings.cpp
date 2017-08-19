#include "projectsettings.h"

void ProjectSettings::loadProjectSettings(const QString projectFileName)
{
    QFile file(projectFileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream stream (&file);

    QString line = findSavedProjectLine(stream, "Version:");
    if (line != "")
        this->version = line.split(':')[1].toInt();

    if (this->version == 120)
    {
        line = findSavedProjectLine(stream, "SliceMode:");
        int selectedMode = line.split(':')[1].toInt();
        if (selectedMode == 1)
            this->modeName = ModeName_Grid;
        else if (selectedMode == 2)
            this->modeName = ModeName_Steps;
    }
    else if (this->version >= 130)
    {
        line = findSavedProjectLine(stream, "ModeName:");
        this->modeName = line.split(':')[1];
        line = findSavedProjectLine(stream, "NormalizationMode:");
        this->normalizationMode = line.split(':')[1].toInt();
        line = findSavedProjectLine(stream, "FadeIn:");
        this->fadein = line.split(':')[1].toInt();
        line = findSavedProjectLine(stream, "FadeOut:");
        this->fadeout = line.split(':')[1].toInt();
        line = findSavedProjectLine(stream, "IncludeTail:");
        this->includeTail = line.split(':')[1] == "1";
        line = findSavedProjectLine(stream, "MegabreakFiles:");
        this->megabreakFiles = line.split(':')[1].toInt();
    }

    line = findSavedProjectLine(stream, "Slicecount:");
    this->sampleCount = line.split(':')[1].toInt();

    for (int i = 0; i < sampleCount; i++)
        this->samplePaths.append(stream.readLine());

    line = findSavedProjectLine(stream, "ChannelCount:");
    this->channelCount = line.split(':')[1].toInt();

    line = findSavedProjectLine(stream, "SampleRate:");
    this->sampleRate = line.split(':')[1].toInt();

    line = findSavedProjectLine(stream, "BitRate:");
    this->bitRate = line.split(':')[1].toInt();

    line = findSavedProjectLine(stream, "Loopsetting:");
    this->loopSetting = line.split(':')[1].toInt();

    line = findSavedProjectLine(stream, "Stretchsetting:");
    this->stretchSetting = line.split(':')[1].toInt();

    if (version >= 120)
    {
        line = findSavedProjectLine(stream, "TrigQuantSetting:");
        this->trigQuantSetting = line.split(':')[1].toInt();

        line = findSavedProjectLine(stream, "Gain:");
        this->gain = line.split(':')[1].toInt();

        line = findSavedProjectLine(stream, "Tempo:");
        this->tempo = line.split(':')[1].toInt();

        line = findSavedProjectLine(stream, "SliceSteps:");
        this->sliceSteps = line.split(':')[1].toInt();

        line = findSavedProjectLine(stream, "CreateOTFile:");
        this->createOTFile = line.split(':')[1].toInt() > 0;
    }
    file.close();
}

void ProjectSettings::saveProjectSettings(const QString projectFileName)
{
    QFile file(projectFileName);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);

    stream << "Version:" << this->currentVersion << "\n";
    stream << "ModeName:" << this->modeName << "\n";
    stream << "ChannelCount:" << this->channelCount << "\n";
    stream << "SampleRate:" << this->sampleRate << "\n";
    stream << "BitRate:" << this->bitRate << "\n";
    stream << "Loopsetting:" << this->loopSetting << "\n";
    stream << "Stretchsetting:" << this->stretchSetting << "\n";
    stream << "TrigQuantSetting:" << this->trigQuantSetting << "\n";
    stream << "Gain:" << this->gain << "\n";
    stream << "Tempo:" << this->tempo << "\n";
    stream << "NormalizationMode:" << this->normalizationMode << "\n";
    stream << "FadeIn:" << this->fadein << "\n";
    stream << "FadeOut:" << this->fadeout << "\n";
    stream << "MegabreakFiles:" << this->megabreakFiles << "\n";
    stream << "SliceSteps:" << this->sliceSteps << "\n";
    stream << "CreateOTFile:" << this->createOTFile << "\n";
    stream << "IncludeTail:" << this->includeTail << "\n";
    stream << "Slicecount:" << this->sampleCount << "\n";
    for (int i = 0; i < sampleCount; i++)
        stream << this->samplePaths[i] << "\n";

    file.close();
}

QString ProjectSettings::findSavedProjectLine(QTextStream &stream, QString searchText)
{
    stream.seek(0);
    QString line = NULL;
    bool found = false;
    while (!stream.atEnd()) {
        line = stream.readLine();
        if (line.contains(searchText))
        {
            found = true;
            break;
        }
    }
    if (!found)
        line = "";
    return line;
}
