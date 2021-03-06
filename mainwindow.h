#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "projectsettings.h"
#include "tabmainwidget.h"
#include "tabgridwidget.h"
#include "tabstepswidget.h"
#include "tabmegabreakwidget.h"

extern QString _defaultPathAudio;
extern QString _defaultPathProjects;
extern QString _defaultPathOutput;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_openProject_triggered();
    void on_actionSave_triggered();
    void on_actionNew_triggered();
    void on_menuAudioPath_triggered();
    void on_actionAbout_triggered();
    void on_actionSet_projects_default_path_triggered();
    void on_actionSet_output_default_path_triggered();

private:
    Ui::MainWindow *ui;
    TabMainWidget *mainTab;
    TabGridWidget *gridTab;
    TabStepsWidget *stepsTab;
    TabMegabreakWidget *megabreakTab;
    void loadFile(const QString &fileName);
    void clearProject();
    void writeOptions();
    void readOptions();
    QString findSavedProjectLine(QTextStream &stream, QString searchText);
};

#endif // MAINWINDOW_H
