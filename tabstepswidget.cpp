#include "tabstepswidget.h"
#include "ui_tabstepswidget.h"

#include "tabmainwidget.h"
#include "ui_tabmainwidget.h"

TabStepsWidget::TabStepsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabStepsWidget)
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

    ui->dropNormalize->addItem("No normalization", NormalizationMode_t::None);
    ui->dropNormalize->addItem("Normalize slices", NormalizationMode_t::Slice);
    ui->dropNormalize->addItem("Normalize chain", NormalizationMode_t::Chain);

    ui->dropSlicePerSteps->setEnabled(true);
    ui->btnCreate->setEnabled(false);

    QShortcut *playHotkey = new QShortcut(QKeySequence(" "), this);
    QObject::connect(playHotkey, SIGNAL(activated()), this, SLOT(on_btnPlay_clicked()));
    mediaplayer = new QMediaPlayer;
}

TabStepsWidget::~TabStepsWidget()
{
    delete ui;
    delete mediaplayer;
}

void TabStepsWidget::playAudio()
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

void TabStepsWidget::createWav(QString filename)
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
    SliceMode_t sliceMode = SliceMode_t::StepsMode;
    NormalizationMode_t normalizationMode = static_cast<NormalizationMode_t>(ui->dropNormalize->currentData().toInt());

    QVector<QString> sourceFiles;
    for (int i = 0; i < ui->listSlices->count(); i++)
    {
        QString itemText = ui->listSlices->item(i)->text();
        sourceFiles.append(itemText.split(" [")[0]);
    }

    QThread *workThread = new QThread;
    AudioFactory *af = new AudioFactory;
    af->setUISelections(sampleRate, bitRate, channels, sourceFiles, filename, loopSetting, stretchSetting, trigQuantSetting, gain, tempo, steps, sliceMode, normalizationMode, true);
    af->moveToThread(workThread);
    connect(workThread, SIGNAL(started()), af, SLOT(generateFiles()));
    connect(af, SIGNAL(doneGenerating()), workThread, SLOT(quit()));
    connect(af, SIGNAL(doneGenerating()), this, SLOT(updateUIafterCreateDone()));
    connect(af, SIGNAL(fileProgress(int, int)), this, SLOT(updateProgressBar(int, int)));
    connect(workThread, SIGNAL(finished()), af, SLOT(deleteLater()));
    connect(workThread, SIGNAL(finished()), workThread, SLOT(deleteLater()));
    workThread->start();
}

void TabStepsWidget::updateProgressBar(int currentSlice, int totalSlices)
{
    int percentage = 100*currentSlice/totalSlices;
    ui->progressBar->setValue(percentage);
}

void TabStepsWidget::updateUIafterCreateDone()
{
    ui->progressBar->setVisible(false);
    ui->progressBar->setValue(0);
    ui->btnCreate->setEnabled(true);
}

void TabStepsWidget::updateSliceCount()
{
    int count = ui->listSlices->count();
    ui->lblSliceCOunt->setText(QString::number(count));
    if (count < 1)
        ui->btnCreate->setEnabled(false);
    else if (count > 64)
        ui->btnCreate->setEnabled(false);
    else
        ui->btnCreate->setEnabled(true);
}

void TabStepsWidget::on_btnAddWav_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Select audio files", _defaultPathAudio, "audio files (*.wav *.aif *.ogg *.flac *.iff *.svx)");
    addListItems(fileNames);
    updateSliceCount();
}

void TabStepsWidget::on_btnPlay_clicked()
{
    if (ui->listSlices->selectedItems().count() > 0)
        playAudio();
}

void TabStepsWidget::on_listSlices_doubleClicked(const QModelIndex &index)
{
    playAudio();
}

void TabStepsWidget::on_btnStop_clicked()
{
    mediaplayer->stop();
}

void TabStepsWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void TabStepsWidget::dropEvent(QDropEvent *event)
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

void TabStepsWidget::on_btnRemove_clicked()
{
    if (ui->listSlices->selectedItems().count() == 1)
        delete ui->listSlices->selectedItems()[0];
    updateSliceCount();
}

void TabStepsWidget::on_btnCreate_clicked()
{
    if (ui->listSlices->count() > 0)
    {
        QString destinationFile = QFileDialog::getSaveFileName(this, "Save as...", _defaultPathOutput, "Wave file (*.wav)");
        if (!destinationFile.isEmpty())
            createWav(destinationFile);
    }
}

void TabStepsWidget::addListItems(const QStringList files)
{
    for (int i = 0; i < files.length(); i++)
    {
        ui->listSlices->addItem(files[i] + AudioFactory::getFormatString(files[i]));
    }
}

void TabStepsWidget::on_sliderGain_valueChanged(int value)
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

void TabStepsWidget::on_sliderBPM_valueChanged(int value)
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

void TabStepsWidget::on_txtBPMValue_textChanged()
{
    QString txtValue = ui->txtBPMValue->toPlainText();
    int sliderValue = (int)(txtValue.toFloat() * 4.0 + 0.5);
    if (sliderValue != ui->sliderBPM->value() && sliderValue >= 30*4 && sliderValue <= 300*4)
        ui->sliderBPM->setValue(sliderValue);
}

void TabStepsWidget::on_txtGainValue_textChanged()
{
    QString txtValue = ui->txtGainValue->toPlainText();
    int sliderValue = (int)(txtValue.toFloat() * 2.0);
    if (sliderValue != ui->sliderGain->value() && sliderValue >= -24*2 && sliderValue <= 24*2)
        ui->sliderGain->setValue(sliderValue);
}

void TabStepsWidget::reset()
{
    ui->listSlices->clear();
    ui->radio16->setChecked(true);
    ui->radio44->setChecked(true);
    ui->radioStereo->setChecked(true);
    ui->dropLoop->setCurrentIndex(0);
    ui->dropStretch->setCurrentIndex(0);
    ui->dropQuant->setCurrentIndex(0);
    ui->sliderGain->setValue(0);
    ui->sliderBPM->setValue(500);
    ui->btnCreate->setEnabled(false);
    ui->dropNormalize->setCurrentIndex(0);
    ui->dropSlicePerSteps->setCurrentIndex(11);
    on_sliderGain_valueChanged(0);
    on_sliderBPM_valueChanged(500);
}

void TabStepsWidget::configure(ProjectSettings &settings)
{
    for (int i = 0; i < settings.sampleCount; i++)
        ui->listSlices->addItem(settings.samplePaths[i]);

    ui->radioMono->setChecked(settings.channelCount == 1);
    if (settings.sampleRate == 32000)
        ui->radio32->setChecked(true);
    else if (settings.sampleRate == 22050)
        ui->radio22->setChecked(true);
    else if (settings.sampleRate == 48000)
        ui->radio48->setChecked(true);
    else
        ui->radio44->setChecked(true);
    ui->radio24->setChecked(settings.bitRate == 24);
    ui->dropLoop->setCurrentIndex(settings.loopSetting);
    ui->dropStretch->setCurrentIndex(settings.stretchSetting);
    ui->dropQuant->setCurrentIndex(settings.trigQuantSetting);
    ui->sliderGain->setValue(settings.gain);
    ui->sliderBPM->setValue(settings.tempo);
    ui->dropNormalize->setCurrentIndex(settings.normalizationMode);
    ui->dropSlicePerSteps->setCurrentIndex(settings.sliceSteps);

    if (ui->listSlices->count() > 0 && ui->listSlices->count() < 65)
        ui->btnCreate->setEnabled(true);

    updateSliceCount();
}

void TabStepsWidget::updateCurrentSettings(ProjectSettings &settings)
{
    settings.modeName = settings.ModeName_Steps;
    settings.sampleCount = ui->listSlices->count();
    settings.bitRate = ui->radio16->isChecked() ? 16 : 24;
    settings.channelCount = ui->radioMono->isChecked() ? 1 : 2;
    settings.sampleRate = 44100;
    if (ui->radio22->isChecked())
        settings.sampleRate = 22050;
    else if (ui->radio32->isChecked())
        settings.sampleRate = 32000;
    else if (ui->radio48->isChecked())
        settings.sampleRate = 48000;
    settings.loopSetting = ui->dropLoop->currentIndex();
    settings.stretchSetting = ui->dropStretch->currentIndex();
    settings.trigQuantSetting = ui->dropQuant->currentIndex();
    settings.gain = ui->sliderGain->value();
    settings.tempo = ui->sliderBPM->value();
    settings.normalizationMode = ui->dropNormalize->currentIndex();
    settings.sliceSteps = ui->dropSlicePerSteps->currentIndex();
    for (int i = 0; i < ui->listSlices->count(); i++)
        settings.samplePaths.append(ui->listSlices->item(i)->text());
}
