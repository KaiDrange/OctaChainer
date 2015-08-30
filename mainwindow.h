#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudio>
#include <QMediaPlayer>
#include "audiofactory.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateProgressBar(int currentSlice, int totalSlices);
    void updateUIafterCreateDone();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void on_openProject_triggered();
    void on_btnAddWav_clicked();
    void on_btnPlay_clicked();
    void on_btnStop_clicked();
    void on_listSlices_doubleClicked(const QModelIndex &index);
    void on_btnRemove_clicked();
    void on_btnCreate_clicked();
    void on_actionSave_triggered();
    void on_actionNew_triggered();
    void on_menuAudioPath_triggered();
    void on_actionAbout_triggered();
    void on_actionSet_projects_default_path_triggered();
    void on_actionSet_output_default_path_triggered();
    void on_sliderGain_valueChanged(int value);
    void on_sliderBPM_valueChanged(int value);
    void on_radioSliceNormal_clicked();
    void on_radioSliceGrid_clicked();
    void on_radioSliceSteps_clicked();
    void on_btnAddSilence_clicked();

private:
    Ui::MainWindow *ui;
    void loadFile(const QString &fileName);
    void playAudio();
    void createWav(const QString destFilename);
    void updateSliceCount();
    void clearProject();
    void writeOptions();
    void readOptions();
    QString findSavedProjectLine(QTextStream &stream, QString searchText);
    void addListItems(const QStringList files);
    QMediaPlayer *mediaplayer;

    QString defaultPathAudio;
    QString defaultPathProjects;
    QString defaultPathOutput;
};

#endif // MAINWINDOW_H
