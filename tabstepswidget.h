#ifndef TABSTEPSWIDGET_H
#define TABSTEPSWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QShortcut>
#include <QThread>
#include <QMimeData>
#include <QAudio>
#include <QMediaPlayer>
#include "projectsettings.h"
#include "audiofactory.h"

extern QString _defaultPathAudio;
extern QString _defaultPathProjects;
extern QString _defaultPathOutput;

namespace Ui {
class TabStepsWidget;
}

class TabStepsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TabStepsWidget(QWidget *parent = 0);
    ~TabStepsWidget();
    void reset();
    void configure(ProjectSettings &settings);
    void updateCurrentSettings(ProjectSettings &settings);

public slots:
    void updateProgressBar(int currentSlice, int totalSlices);
    void updateUIafterCreateDone();

protected:

private slots:
    void on_btnPlay_clicked();
    void on_btnStop_clicked();
    void on_btnCreate_clicked();
    void on_sliderGain_valueChanged(int value);
    void on_sliderBPM_valueChanged(int value);
    void on_txtBPMValue_textChanged();
    void on_txtGainValue_textChanged();
    void on_btnSelectFile_clicked();

    void on_dropSlicePerSteps_currentIndexChanged(int index);

    void on_chkTail_stateChanged(int arg1);

private:
    Ui::TabStepsWidget *ui;
    void playAudio();
    void createOTFile();
    void updateSliceCount();
    QMediaPlayer *mediaplayer;
    int sliceCount;
    bool hasTail;

};

#endif // TABSTEPSWIDGET_H
