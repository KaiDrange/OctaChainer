#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progressBar->setVisible(false);

    ui->dropLoop->addItem("Loop off", Loop_t::NoLoop);
    ui->dropLoop->addItem("Loop on", Loop_t::Loop);
    ui->dropLoop->addItem("Loop pingpong", Loop_t::PIPO);

    ui->dropStretch->addItem("Timestretch off", Stretch_t::NoStrech);
    ui->dropStretch->addItem("TimeStretch normal", Stretch_t::Normal);
    ui->dropStretch->addItem("TimeStretch beat", Stretch_t::Beat);

    ui->dropQuant->addItem("Trig quant direct", TrigQuant_t::Direct);
    ui->dropQuant->addItem("Trig quant pattern", TrigQuant_t::Pattern);
    ui->dropQuant->addItem("Trig quant 1", TrigQuant_t::S_1);
    ui->dropQuant->addItem("Trig quant 2", TrigQuant_t::S_2);
    ui->dropQuant->addItem("Trig quant 3", TrigQuant_t::S_3);
    ui->dropQuant->addItem("Trig quant 4", TrigQuant_t::S_4);
    ui->dropQuant->addItem("Trig quant 6", TrigQuant_t::S_6);
    ui->dropQuant->addItem("Trig quant 8", TrigQuant_t::S_8);
    ui->dropQuant->addItem("Trig quant 12", TrigQuant_t::S_12);
    ui->dropQuant->addItem("Trig quant 16", TrigQuant_t::S_16);
    ui->dropQuant->addItem("Trig quant 24", TrigQuant_t::S_24);
    ui->dropQuant->addItem("Trig quant 32", TrigQuant_t::S_32);
    ui->dropQuant->addItem("Trig quant 48", TrigQuant_t::S_48);
    ui->dropQuant->addItem("Trig quant 64", TrigQuant_t::S_64);
    ui->dropQuant->addItem("Trig quant 96", TrigQuant_t::S_96);
    ui->dropQuant->addItem("Trig quant 128", TrigQuant_t::S_128);
    ui->dropQuant->addItem("Trig quant 192", TrigQuant_t::S_192);
    ui->dropQuant->addItem("Trig quant 256", TrigQuant_t::S_256);

    ui->dropSlicePerSteps->addItem("Slice every 1 step", 1);
    ui->dropSlicePerSteps->addItem("Slice every 2 steps", 2);
    ui->dropSlicePerSteps->addItem("Slice every 3 steps", 3);
    ui->dropSlicePerSteps->addItem("Slice every 4 steps", 4);
    ui->dropSlicePerSteps->addItem("Slice every 6 steps", 6);
    ui->dropSlicePerSteps->addItem("Slice every 8 steps", 8);
    ui->dropSlicePerSteps->addItem("Slice every 12 steps", 12);
    ui->dropSlicePerSteps->addItem("Slice every 16 steps", 16);
    ui->dropSlicePerSteps->addItem("Slice every 24 steps", 24);
    ui->dropSlicePerSteps->addItem("Slice every 32 steps", 32);
    ui->dropSlicePerSteps->addItem("Slice every 48 steps", 48);
    ui->dropSlicePerSteps->addItem("Slice every 64 steps", 64);
    ui->dropSlicePerSteps->addItem("Slice every 96 steps", 96);
    ui->dropSlicePerSteps->addItem("Slice every 128 steps", 128);
    ui->dropSlicePerSteps->addItem("Slice every 192 steps", 192);
    ui->dropSlicePerSteps->addItem("Slice every 256 steps", 256);
    ui->dropSlicePerSteps->setCurrentIndex(11);

    ui->dropSlicePerSteps->setEnabled(false);

    QShortcut *playHotkey = new QShortcut(QKeySequence(" "), this);
    QObject::connect(playHotkey, SIGNAL(activated()), this, SLOT(on_btnPlay_clicked()));

    setAcceptDrops(true);
    readOptions();
    ui->btnCreate->setEnabled(false);

    mediaplayer = new QMediaPlayer;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mediaplayer;
}

void MainWindow::playAudio()
{
    if (!mediaplayer->StoppedState)
        mediaplayer->stop();

    QString itemText = ui->listSlices->selectedItems()[0]->text();
    if (itemText != SILENT_SLICE_NAME)
    {
        mediaplayer->setMedia(QUrl::fromLocalFile(itemText.split(" [")[0]));
        mediaplayer->play();
    }
}

void MainWindow::createWav(QString filename)
{
    ui->btnCreate->setEnabled(false);
    ui->progressBar->setVisible(true);

    int channels = ui->radioMono->isChecked() ? 1 : 2;
    int bitRate = ui->radio16->isChecked() ? 16 : 24;
    int sampleRate = 44100;
    if (ui->radio32->isChecked())
        sampleRate = 32000;
    else if (ui->radio22->isChecked())
        sampleRate = 22050;
    else if (ui->radio48->isChecked())
        sampleRate = 48000;
    Loop_t loopSetting = static_cast<Loop_t>(ui->dropLoop->currentData().toInt());
    Stretch_t stretchSetting = static_cast<Stretch_t>(ui->dropStretch->currentData().toInt());
    TrigQuant_t trigQuantSetting = static_cast<TrigQuant_t>(ui->dropQuant->currentData().toInt());
    int gain = ui->sliderGain->value();
    int tempo = ui->sliderBPM->value();
    int steps = ui->dropSlicePerSteps->currentData().toInt();
    SliceMode_t sliceMode = SliceMode_t::NormalMode;
    if (ui->radioSliceGrid->isChecked())
        sliceMode = SliceMode_t::GridMode;
    else if (ui->radioSliceSteps->isChecked())
            sliceMode = SliceMode_t::StepsMode;

    QVector<QString> sourceFiles;
    for (int i = 0; i < ui->listSlices->count(); i++)
    {
        QString itemText = ui->listSlices->item(i)->text();
        sourceFiles.append(itemText.split(" [")[0]);
    }

    QThread *workThread = new QThread;
    AudioFactory *af = new AudioFactory;
    af->setUISelections(sampleRate, bitRate, channels, sourceFiles, filename, loopSetting, stretchSetting, trigQuantSetting, gain, tempo, steps, sliceMode);
    af->moveToThread(workThread);
    connect(workThread, SIGNAL(started()), af, SLOT(generateFiles()));
    connect(af, SIGNAL(doneGenerating()), workThread, SLOT(quit()));
    connect(af, SIGNAL(doneGenerating()), this, SLOT(updateUIafterCreateDone()));
    connect(af, SIGNAL(fileProgress(int, int)), this, SLOT(updateProgressBar(int, int)));
    connect(workThread, SIGNAL(finished()), af, SLOT(deleteLater()));
    connect(workThread, SIGNAL(finished()), workThread, SLOT(deleteLater()));
    workThread->start();
}

void MainWindow::updateProgressBar(int currentSlice, int totalSlices)
{
    int percentage = 100*currentSlice/totalSlices;
    ui->progressBar->setValue(percentage);
}

void MainWindow::updateUIafterCreateDone()
{
    ui->progressBar->setVisible(false);
    ui->progressBar->setValue(0);
    ui->btnCreate->setEnabled(true);
}

void MainWindow::updateSliceCount()
{
    int count = ui->listSlices->count();
    ui->lblSliceCOunt->setText(QString::number(count));
    if (count < 1)
        ui->btnCreate->setEnabled(false);
    else if (!ui->radioSliceGrid->isChecked() && count > 64)
        ui->btnCreate->setEnabled(false);
    else
        ui->btnCreate->setEnabled(true);
}

void MainWindow::on_btnAddWav_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Select audio files", this->defaultPathAudio, "audio files (*.wav *.aif *.ogg *.flac *.iff *.svx)");
    addListItems(fileNames);
    updateSliceCount();
}

void MainWindow::on_btnPlay_clicked()
{
    if (ui->listSlices->selectedItems().count() > 0)
        playAudio();
}

void MainWindow::on_listSlices_doubleClicked(const QModelIndex &index)
{
    playAudio();
}

void MainWindow::on_btnStop_clicked()
{
    mediaplayer->stop();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    QStringList fileList;
    for (int i = 0; i < urls.count(); i++)
    {
        QString localFile = urls[i].toLocalFile();
        if (
                localFile.endsWith(".wav", Qt::CaseInsensitive) ||
                localFile.endsWith(".aif", Qt::CaseInsensitive) ||
                localFile.endsWith(".ogg", Qt::CaseInsensitive) ||
                localFile.endsWith(".flac", Qt::CaseInsensitive) ||
                localFile.endsWith(".iff", Qt::CaseInsensitive) ||
                localFile.endsWith(".svx", Qt::CaseInsensitive)
            )
            fileList.append(urls[i].toLocalFile());
    }
    if (fileList.count() > 0)
    {
        addListItems(fileList);
        updateSliceCount();
    }
}

void MainWindow::on_btnRemove_clicked()
{
    if (ui->listSlices->selectedItems().count() == 1)
        delete ui->listSlices->selectedItems()[0];
    updateSliceCount();
}

void MainWindow::on_btnCreate_clicked()
{
    if (ui->listSlices->count() > 0)
    {
        QString destinationFile = QFileDialog::getSaveFileName(this, "Save as...", this->defaultPathOutput, "Wave file (*.wav)");
        if (!destinationFile.isEmpty())
            createWav(destinationFile);
    }
}

void MainWindow::on_actionSave_triggered()
{
    QString destinationFile = QFileDialog::getSaveFileName(this, "Save as...", this->defaultPathProjects, "OctaChainer project (*.ocp)");
    if (!destinationFile.isEmpty())
    {
        int sliceCount = this->ui->listSlices->count();
        int bitRate = ui->radio16->isChecked() ? 16 : 24;
        int channelCount = ui->radioMono->isChecked() ? 1 : 2;
        int sampleRate = 44100;
        if (ui->radio22->isChecked())
            sampleRate = 22050;
        else if (ui->radio32->isChecked())
            sampleRate = 32000;
        else if (ui->radio48->isChecked())
            sampleRate = 48000;

        SliceMode_t sliceMode = SliceMode_t::NormalMode;
        if (ui->radioSliceGrid->isChecked())
            sliceMode = SliceMode_t::GridMode;
        else if (ui->radioSliceSteps->isChecked())
            sliceMode = SliceMode_t::StepsMode;

        QFile file(destinationFile);
        file.open( QIODevice::WriteOnly );
        QTextStream stream( &file );
        stream << "Version:120\n";
        stream << "ChannelCount:" << channelCount << "\n";
        stream << "SampleRate:" << sampleRate << "\n";
        stream << "BitRate:" << bitRate << "\n";
        stream << "Loopsetting:" << ui->dropLoop->currentIndex() << "\n";
        stream << "Stretchsetting:" << ui->dropStretch->currentIndex() << "\n";
        stream << "TrigQuantSetting:" << ui->dropQuant->currentIndex() << "\n";
        stream << "Gain:" << ui->sliderGain->value() << "\n";
        stream << "Tempo:" << ui->sliderBPM->value() << "\n";
        stream << "SliceMode:" << sliceMode << "\n";
        stream << "SliceSteps:" << ui->dropSlicePerSteps->currentIndex() << "\n";
        stream << "Slicecount:" << sliceCount << "\n";
        for (int i = 0; i < sliceCount; i++)
            stream << ui->listSlices->item(i)->text() << "\n";
        file.close();
    }
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
    QString fileName = QFileDialog::getOpenFileName(this, "Select project file", this->defaultPathProjects, "Octachainer project file (*.ocp)");
    if (!fileName.isEmpty())
    {
        clearProject();
        QFile file(fileName);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream stream (&file);

        int version = 0;
        QString line = findSavedProjectLine(stream, "Version:");
        if (line != "")
            version = line.split(':')[1].toInt();

        line = findSavedProjectLine(stream, "Slicecount:");
        int sampleCount = line.split(':')[1].toInt();

        for (int i = 0; i < sampleCount; i++)
            ui->listSlices->addItem(stream.readLine());

        line = findSavedProjectLine(stream, "ChannelCount:");
        ui->radioMono->setChecked(line.split(':')[1].toInt() == 1);

        line = findSavedProjectLine(stream, "SampleRate:");
        int bitRate = line.split(':')[1].toInt();
        if (bitRate == 32000)
            ui->radio32->setChecked(true);
        else if (bitRate == 22050)
            ui->radio22->setChecked(true);
        else if (bitRate == 48000)
            ui->radio48->setChecked(true);

        line = findSavedProjectLine(stream, "BitRate:");
        ui->radio24->setChecked(line.split(':')[1].toInt() == 24);

        line = findSavedProjectLine(stream, "Loopsetting:");
        ui->dropLoop->setCurrentIndex(line.split(':')[1].toInt());

        line = findSavedProjectLine(stream, "Stretchsetting:");
        ui->dropStretch->setCurrentIndex(line.split(':')[1].toInt());

        if (version >= 120)
        {
            line = findSavedProjectLine(stream, "TrigQuantSetting:");
            ui->dropQuant->setCurrentIndex(line.split(':')[1].toInt());

            line = findSavedProjectLine(stream, "Gain:");
            ui->sliderGain->setValue(line.split(':')[1].toInt());

            line = findSavedProjectLine(stream, "Tempo:");
            ui->sliderBPM->setValue(line.split(':')[1].toInt());

            line = findSavedProjectLine(stream, "SliceMode:");
            int selectedMode = line.split(':')[1].toInt();
            if (selectedMode == 1)
                ui->radioSliceGrid->setChecked(true);
            else if (selectedMode == 2)
                ui->radioSliceSteps->setChecked(true);

            line = findSavedProjectLine(stream, "SliceSteps:");
            ui->dropSlicePerSteps->setCurrentIndex(line.split(':')[1].toInt());
        }
        if (ui->listSlices->count() > 0 && ui->listSlices->count() < 65)
            ui->btnCreate->setEnabled(true);

        updateSliceCount();
    }
}

void MainWindow::on_actionNew_triggered()
{
    clearProject();
}

void MainWindow::clearProject()
{
    ui->listSlices->clear();
    ui->radio16->setChecked(true);
    ui->radio44->setChecked(true);
    ui->radioStereo->setChecked(true);
    ui->dropLoop->setCurrentIndex(0);
    ui->dropStretch->setCurrentIndex(0);
    ui->btnCreate->setEnabled(false);
    ui->dropSlicePerSteps->setCurrentIndex(11);
    ui->dropSlicePerSteps->setEnabled(false);
    ui->radioSliceNormal->setChecked(true);
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

void MainWindow::addListItems(const QStringList files)
{
    for (int i = 0; i < files.length(); i++)
    {
        ui->listSlices->addItem(files[i] + AudioFactory::getFormatString(files[i]));
    }
}

void MainWindow::on_sliderGain_valueChanged(int value)
{
    QString txtValue = QString::number(value/2.0);
    if (txtValue != ui->txtGainValue->toPlainText())
    {
        ui->txtGainValue->setPlainText(txtValue);
        QString test = ui->txtGainValue->toPlainText();
        QTextCursor cursor = ui->txtGainValue->textCursor();
        cursor.setPosition(txtValue.length());
        ui->txtGainValue->setTextCursor(cursor);
    }
}

void MainWindow::on_sliderBPM_valueChanged(int value)
{
    QString txtValue = QString::number(value/4.0);
    if (txtValue != ui->txtBPMValue->toPlainText())
    {
        ui->txtBPMValue->setPlainText(QString::number(value/4.0));
        QTextCursor cursor = ui->txtBPMValue->textCursor();
        cursor.setPosition(txtValue.length());
        ui->txtBPMValue->setTextCursor(cursor);
    }
}

void MainWindow::on_radioSliceNormal_clicked()
{
    ui->dropSlicePerSteps->setEnabled(false);
    updateSliceCount();
}

void MainWindow::on_radioSliceGrid_clicked()
{
    ui->dropSlicePerSteps->setEnabled(false);
    updateSliceCount();
}

void MainWindow::on_radioSliceSteps_clicked()
{
    ui->dropSlicePerSteps->setEnabled(true);
    updateSliceCount();
}

void MainWindow::on_btnAddSilence_clicked()
{
    ui->listSlices->addItem(SILENT_SLICE_NAME);
    updateSliceCount();
}

void MainWindow::on_txtBPMValue_textChanged()
{
    QString txtValue = ui->txtBPMValue->toPlainText();
    int sliderValue = (int)(txtValue.toFloat() * 4.0 + 0.5);
    if (sliderValue != ui->sliderBPM->value() && sliderValue >= 30*4 && sliderValue <= 300*4)
        ui->sliderBPM->setValue(sliderValue);
}

void MainWindow::on_txtGainValue_textChanged()
{
    QString txtValue = ui->txtGainValue->toPlainText();
    int sliderValue = (int)(txtValue.toFloat() * 2.0);
    if (sliderValue != ui->sliderGain->value() && sliderValue >= -24*2 && sliderValue <= 24*2)
        ui->sliderGain->setValue(sliderValue);
}
