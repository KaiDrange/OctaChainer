#ifndef AUDIOFACTORY_H
#define AUDIOFACTORY_H

#include <stdlib.h>
#include <limits>
#include <QObject>
#include <QString>
#include <QVector>
#include <sndfile.hh>
#include "otwriter.h"

#define	BUFFER_LENGTH	4096
#define SILENT_SLICE_NAME "--- silent slice (1 second in length outside grid mode) ---"

enum SliceMode_t { NormalMode = 0, GridMode = 1, StepsMode = 2 };
enum NormalizationMode_t { None = 0, Slice = 1, Chain = 2 };

class AudioFactory : public QObject
{
    Q_OBJECT

public slots:
    void generateFiles();
public:
    static QString getFormatString(const QString &file);
    void setUISelections(const int sampleRate,
                         const int bitRate,
                         const int channels,
                         const QVector<QString> sourceFiles,
                         const QString &outFileName,
                         const Loop_t loopSetting,
                         const Stretch_t stretchSetting,
                         const TrigQuant_t trigQuantSetting,
                         const int gain,
                         const int tempo,
                         const int steps,
                         const SliceMode_t sliceMode,
                         const NormalizationMode_t normalizationMode);

signals:
    void doneGenerating();
    void fileProgress(int currentSlice, int totalSlices);
private slots:
private:
    void createOutput_NormalMode();
    void createOutput_GridMode();
    void createOutput_StepsMode();
    double findNormalizationFactor(SndfileHandle *file);
    void normalizeChain();
    int buffer[BUFFER_LENGTH];
    OTWriter *otWriter;
    int currentFileNo;
    uint32_t currentFileStartPoint;

    int sampleRate;
    int bitRate;
    int channels;
    QVector<QString> sourceFiles;
    QString outFileName;
    Loop_t loopSetting;
    Stretch_t stretchSetting;
    TrigQuant_t trigQuantSetting;
    int gain;
    int tempo;
    int steps;
    SliceMode_t sliceMode;
    NormalizationMode_t normalizationMode;
};

#endif // AUDIOFACTORY_H

