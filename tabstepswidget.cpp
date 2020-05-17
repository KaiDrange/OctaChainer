#include "tabstepswidget.h"
#include "ui_tabstepswidget.h"
#include "audioutil.h"

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

    QString filename = ui->txtAudiofile->text();
    if (filename != "")
    {
        mediaplayer->setMedia(QUrl::fromLocalFile(filename));
        mediaplayer->play();
    }
}

void TabStepsWidget::createOTFile()
{
    ui->btnCreate->setEnabled(false);
    ui->progressBar->setVisible(true);

    Loop_t loopSetting = static_cast<Loop_t>(ui->dropLoop->currentData().toInt());
    Stretch_t stretchSetting = static_cast<Stretch_t>(ui->dropStretch->currentData().toInt());
    TrigQuant_t trigQuantSetting = static_cast<TrigQuant_t>(ui->dropQuant->currentData().toInt());
    int gain = ui->sliderGain->value();
    int tempo = ui->sliderBPM->value();
    int steps = ui->dropSlicePerSteps->currentData().toInt();
    bool includeTail = ui->chkTail->checkState();

    QThread *workThread = new QThread;
    AudioFactory *af = new AudioFactory;
    af->setStepsModeUISelections(ui->txtAudiofile->text(), loopSetting, stretchSetting, trigQuantSetting, gain, tempo, steps, includeTail);
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
    QString sliceText = "0";
    if (ui->txtAudiofile->text() != "")
    {
        int tempo = ui->sliderBPM->value();
        int steps = ui->dropSlicePerSteps->currentData().toInt();
        bool includeTail = ui->chkTail->checkState();
        AudioFactory::getStepsModeSliceCount(steps, tempo, includeTail, ui->txtAudiofile->text(), this->sliceCount, this->hasTail);

        sliceText = QString::number(this->sliceCount);
        if (this->hasTail)
            sliceText += " + tail";
    }
    ui->lblSliceCount->setText(sliceText);

}

void TabStepsWidget::on_btnPlay_clicked()
{
    playAudio();
}

void TabStepsWidget::on_btnStop_clicked()
{
    mediaplayer->stop();
}

void TabStepsWidget::on_btnCreate_clicked()
{
    if (ui->txtAudiofile->text() != "")
        createOTFile();
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
    updateSliceCount();
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
    ui->txtAudiofile->setText("");
    ui->chkTail->setChecked(true);
    this->sliceCount = 0;
    this->hasTail = false;

    ui->dropLoop->setCurrentIndex(0);
    ui->dropStretch->setCurrentIndex(0);
    ui->dropQuant->setCurrentIndex(0);
    ui->sliderGain->setValue(0);
    ui->sliderBPM->setValue(500);
    ui->btnCreate->setEnabled(false);
    ui->dropSlicePerSteps->setCurrentIndex(11);
    on_sliderGain_valueChanged(0);
    on_sliderBPM_valueChanged(500);
}

void TabStepsWidget::configure(ProjectSettings &settings)
{
    if (settings.sampleCount > 0)
        ui->txtAudiofile->setText(settings.samplePaths[0]);
    else
        ui->txtAudiofile->setText("");
    ui->chkTail->setChecked(settings.includeTail);
    ui->dropLoop->setCurrentIndex(settings.loopSetting);
    ui->dropStretch->setCurrentIndex(settings.stretchSetting);
    ui->dropQuant->setCurrentIndex(settings.trigQuantSetting);
    ui->sliderGain->setValue(settings.gain);
    ui->sliderBPM->setValue(settings.tempo);
    ui->dropSlicePerSteps->setCurrentIndex(settings.sliceSteps);

    updateSliceCount();
}

void TabStepsWidget::updateCurrentSettings(ProjectSettings &settings)
{
    settings.modeName = settings.ModeName_Steps;
    settings.loopSetting = ui->dropLoop->currentIndex();
    settings.stretchSetting = ui->dropStretch->currentIndex();
    settings.trigQuantSetting = ui->dropQuant->currentIndex();
    settings.gain = ui->sliderGain->value();
    settings.tempo = ui->sliderBPM->value();
    settings.sliceSteps = ui->dropSlicePerSteps->currentIndex();
    settings.includeTail = ui->chkTail->checkState();
    if (ui->txtAudiofile->text() != "")
    {
        settings.sampleCount = 1;
        settings.samplePaths.append(ui->txtAudiofile->text());
    }
    else
        settings.sampleCount = 0;
}

void TabStepsWidget::on_btnSelectFile_clicked()
{
    ui->txtAudiofile->setText(QFileDialog::getOpenFileName(this, "Select audio file", _defaultPathAudio, AudioUtil::audioFileFilter()));
    ui->btnCreate->setEnabled(ui->txtAudiofile->text() != "");
    updateSliceCount();
}

void TabStepsWidget::on_dropSlicePerSteps_currentIndexChanged(int index)
{
    updateSliceCount();
}

void TabStepsWidget::on_chkTail_stateChanged(int arg1)
{
    updateSliceCount();
}
