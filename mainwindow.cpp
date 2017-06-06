#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainTab = new TabMainWidget(this);
    ui->tabWidget->addTab(mainTab, tr("Octatrack slices"));
    gridTab = new TabGridWidget(this);
    ui->tabWidget->addTab(gridTab, tr("Evenly spaced grid"));
    stepsTab = new TabStepsWidget(this);
    ui->tabWidget->addTab(stepsTab, tr("Slice per x step of BPM"));
    megabreakTab = new TabMegabreakWidget(this);
    ui->tabWidget->addTab(megabreakTab, tr("Megabreak of Doom"));

    setAcceptDrops(true);
    readOptions();
    clearProject();
}

MainWindow::~MainWindow()
{
    delete mainTab;
    delete gridTab;
    delete megabreakTab;
    delete ui;
}


void MainWindow::on_actionSave_triggered()
{
    QString destinationFile = QFileDialog::getSaveFileName(this, "Save as...", _defaultPathProjects, "OctaChainer project (*.ocp)");
    if (!destinationFile.isEmpty())
    {
        ProjectSettings ps;
        if (ui->tabWidget->currentIndex() == 0)
            mainTab->updateCurrentSettings(ps);
        else if (ui->tabWidget->currentIndex() == 1)
            gridTab->updateCurrentSettings(ps);
        else if (ui->tabWidget->currentIndex() == 2)
            stepsTab->updateCurrentSettings(ps);
        else if (ui->tabWidget->currentIndex() == 3)
            megabreakTab->updateCurrentSettings(ps);

        ps.saveProjectSettings(destinationFile);
    }
}


void MainWindow::on_openProject_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select project file", _defaultPathProjects, "Octachainer project file (*.ocp)");
    if (!fileName.isEmpty())
    {
        clearProject();
        ProjectSettings ps;
        ps.loadProjectSettings(fileName);
        if (ps.modeName == ps.ModeName_Main)
        {
            mainTab->configure(ps);
            ui->tabWidget->setCurrentIndex(0);
        }
        else if (ps.modeName == ps.ModeName_Grid)
        {
            gridTab->configure(ps);
            ui->tabWidget->setCurrentIndex(1);
        }
        else if (ps.modeName == ps.ModeName_Steps)
        {
            stepsTab->configure(ps);
            ui->tabWidget->setCurrentIndex(2);
        }
        else if (ps.modeName == ps.ModeName_Megabreak)
        {
            gridTab->configure(ps);
            ui->tabWidget->setCurrentIndex(3);
        }
    }
}

void MainWindow::on_actionNew_triggered()
{
    clearProject();
}

void MainWindow::clearProject()
{
    mainTab->reset();
    gridTab->reset();
    stepsTab->reset();
}

void MainWindow::on_menuAudioPath_triggered()
{
    _defaultPathAudio = QFileDialog::getExistingDirectory();
    writeOptions();
}

void MainWindow::writeOptions()
{
    QFile file("options.txt");
    file.open( QIODevice::WriteOnly );
    QTextStream stream( &file );
    stream << _defaultPathProjects << "\n";
    stream << _defaultPathAudio << "\n";
    stream << _defaultPathOutput << "\n";
    file.close();
}

void MainWindow::readOptions()
{
    QFile file("options.txt");
    if (file.open (QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream (&file);
        _defaultPathProjects = stream.readLine();
        _defaultPathAudio = stream.readLine();
        _defaultPathOutput = stream.readLine();
        file.close();
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "OctaChainer v1.3", "Freeware tool for the Elektron Octatrack and Rytm. Created by Elektronauts user Abhoth.");
}

void MainWindow::on_actionSet_projects_default_path_triggered()
{
    _defaultPathProjects = QFileDialog::getExistingDirectory();
    writeOptions();
}

void MainWindow::on_actionSet_output_default_path_triggered()
{
    _defaultPathOutput = QFileDialog::getExistingDirectory();
    writeOptions();
}

