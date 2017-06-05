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
//    QString destinationFile = QFileDialog::getSaveFileName(this, "Save as...", this->defaultPathProjects, "OctaChainer project (*.ocp)");
//    if (!destinationFile.isEmpty())
//    {
//        int sliceCount = this->ui->listSlices->count();
//        int bitRate = ui->radio16->isChecked() ? 16 : 24;
//        int channelCount = ui->radioMono->isChecked() ? 1 : 2;
//        int sampleRate = 44100;
//        if (ui->radio22->isChecked())
//            sampleRate = 22050;
//        else if (ui->radio32->isChecked())
//            sampleRate = 32000;
//        else if (ui->radio48->isChecked())
//            sampleRate = 48000;

//        SliceMode_t sliceMode = SliceMode_t::NormalMode;
//        if (ui->radioSliceGrid->isChecked())
//            sliceMode = SliceMode_t::GridMode;
//        else if (ui->radioSliceSteps->isChecked())
//            sliceMode = SliceMode_t::StepsMode;

//        QFile file(destinationFile);
//        file.open( QIODevice::WriteOnly );
//        QTextStream stream( &file );
//        stream << "Version:120\n";
//        stream << "ChannelCount:" << channelCount << "\n";
//        stream << "SampleRate:" << sampleRate << "\n";
//        stream << "BitRate:" << bitRate << "\n";
//        stream << "Loopsetting:" << ui->dropLoop->currentIndex() << "\n";
//        stream << "Stretchsetting:" << ui->dropStretch->currentIndex() << "\n";
//        stream << "TrigQuantSetting:" << ui->dropQuant->currentIndex() << "\n";
//        stream << "Gain:" << ui->sliderGain->value() << "\n";
//        stream << "Tempo:" << ui->sliderBPM->value() << "\n";
//        stream << "SliceMode:" << sliceMode << "\n";
//        stream << "SliceSteps:" << ui->dropSlicePerSteps->currentIndex() << "\n";
//        stream << "Slicecount:" << sliceCount << "\n";
//        for (int i = 0; i < sliceCount; i++)
//            stream << ui->listSlices->item(i)->text() << "\n";
//        file.close();
//    }
}

QString MainWindow::findSavedProjectLine(QTextStream &stream, QString searchText)
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

void MainWindow::on_openProject_triggered()
{
//    QString fileName = QFileDialog::getOpenFileName(this, "Select project file", this->defaultPathProjects, "Octachainer project file (*.ocp)");
//    if (!fileName.isEmpty())
//    {
//        clearProject();
//        QFile file(fileName);
//        file.open(QIODevice::ReadOnly | QIODevice::Text);
//        QTextStream stream (&file);

//        int version = 0;
//        QString line = findSavedProjectLine(stream, "Version:");
//        if (line != "")
//            version = line.split(':')[1].toInt();

//        line = findSavedProjectLine(stream, "Slicecount:");
//        int sampleCount = line.split(':')[1].toInt();

//        for (int i = 0; i < sampleCount; i++)
//            ui->listSlices->addItem(stream.readLine());

//        line = findSavedProjectLine(stream, "ChannelCount:");
//        ui->radioMono->setChecked(line.split(':')[1].toInt() == 1);

//        line = findSavedProjectLine(stream, "SampleRate:");
//        int bitRate = line.split(':')[1].toInt();
//        if (bitRate == 32000)
//            ui->radio32->setChecked(true);
//        else if (bitRate == 22050)
//            ui->radio22->setChecked(true);
//        else if (bitRate == 48000)
//            ui->radio48->setChecked(true);

//        line = findSavedProjectLine(stream, "BitRate:");
//        ui->radio24->setChecked(line.split(':')[1].toInt() == 24);

//        line = findSavedProjectLine(stream, "Loopsetting:");
//        ui->dropLoop->setCurrentIndex(line.split(':')[1].toInt());

//        line = findSavedProjectLine(stream, "Stretchsetting:");
//        ui->dropStretch->setCurrentIndex(line.split(':')[1].toInt());

//        if (version >= 120)
//        {
//            line = findSavedProjectLine(stream, "TrigQuantSetting:");
//            ui->dropQuant->setCurrentIndex(line.split(':')[1].toInt());

//            line = findSavedProjectLine(stream, "Gain:");
//            ui->sliderGain->setValue(line.split(':')[1].toInt());

//            line = findSavedProjectLine(stream, "Tempo:");
//            ui->sliderBPM->setValue(line.split(':')[1].toInt());

//            line = findSavedProjectLine(stream, "SliceMode:");
//            int selectedMode = line.split(':')[1].toInt();
//            if (selectedMode == 1)
//                ui->radioSliceGrid->setChecked(true);
//            else if (selectedMode == 2)
//                ui->radioSliceSteps->setChecked(true);

//            line = findSavedProjectLine(stream, "SliceSteps:");
//            ui->dropSlicePerSteps->setCurrentIndex(line.split(':')[1].toInt());
//        }
//        if (ui->listSlices->count() > 0 && ui->listSlices->count() < 65)
//            ui->btnCreate->setEnabled(true);

//        updateSliceCount();
//    }
}

void MainWindow::on_actionNew_triggered()
{
    clearProject();
}

void MainWindow::clearProject()
{
//    ui->listSlices->clear();
//    ui->radio16->setChecked(true);
//    ui->radio44->setChecked(true);
//    ui->radioStereo->setChecked(true);
//    ui->dropLoop->setCurrentIndex(0);
//    ui->dropStretch->setCurrentIndex(0);
//    ui->btnCreate->setEnabled(false);
//    ui->dropSlicePerSteps->setCurrentIndex(11);
//    ui->dropSlicePerSteps->setEnabled(false);
//    ui->radioSliceNormal->setChecked(true);
}

void MainWindow::on_menuAudioPath_triggered()
{
    this->defaultPathAudio = QFileDialog::getExistingDirectory();
    writeOptions();
}

void MainWindow::writeOptions()
{
    QFile file("options.txt");
    file.open( QIODevice::WriteOnly );
    QTextStream stream( &file );
    stream << this->defaultPathProjects << "\n";
    stream << this->defaultPathAudio << "\n";
    stream << this->defaultPathOutput << "\n";
    file.close();
}

void MainWindow::readOptions()
{
    QFile file("options.txt");
    if (file.open (QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream (&file);
        this->defaultPathProjects = stream.readLine();
        this->defaultPathAudio = stream.readLine();
        this->defaultPathOutput = stream.readLine();
        file.close();
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "OctaChainer v1.2", "Freeware tool for the Elektron Octatrack and Rytm. Created by Elektronauts user Abhoth.");
}

void MainWindow::on_actionSet_projects_default_path_triggered()
{
    this->defaultPathProjects = QFileDialog::getExistingDirectory();
    writeOptions();
}

void MainWindow::on_actionSet_output_default_path_triggered()
{
    this->defaultPathOutput = QFileDialog::getExistingDirectory();
    writeOptions();
}

