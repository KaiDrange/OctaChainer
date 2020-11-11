#include "tabgridwidget.h"
#include "ui_tabgridwidget.h"
#include "audioutil.h"

TabGridWidget::TabGridWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabGridWidget)
{
    ui->setupUi(this);
    ui->listSlices->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->btnPlay->setEnabled(false);
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

    ui->dropNormalize->addItem("No normalization", NormalizationMode_t::None);
    ui->dropNormalize->addItem("Normalize slices", NormalizationMode_t::Slice);
    ui->dropNormalize->addItem("Normalize chain", NormalizationMode_t::Chain);

    ui->dropFadeIn->addItem("No fade-in", 0);
    ui->dropFadeIn->addItem("1ms fade-in", 1);
    ui->dropFadeIn->addItem("2ms fade-in", 2);
    ui->dropFadeIn->addItem("3ms fade-in", 3);
    ui->dropFadeIn->addItem("5ms fade-in", 5);
    ui->dropFadeIn->addItem("10ms fade-in", 10);

    ui->dropFadeOut->addItem("No fade-out", 0);
    ui->dropFadeOut->addItem("1ms fade-out", 1);
    ui->dropFadeOut->addItem("2ms fade-out", 2);
    ui->dropFadeOut->addItem("3ms fade-out", 3);
    ui->dropFadeOut->addItem("5ms fade-out", 5);
    ui->dropFadeOut->addItem("10ms fade-out", 10);

    ui->btnCreate->setEnabled(false);

    QShortcut *playHotkey = new QShortcut(Qt::Key_Space, this);
    QObject::connect(playHotkey, SIGNAL(activated()), this, SLOT(on_playAudio_toggled()));
    mediaplayer = new QMediaPlayer;
}

TabGridWidget::~TabGridWidget()
{
    delete ui;
    delete mediaplayer;
}

void TabGridWidget::playAudio()
{
    if (mediaplayer->state() == QMediaPlayer::PlayingState)
        mediaplayer->stop();

    QString itemText = ui->listSlices->selectedItems()[0]->text();
    if (itemText != SILENT_SLICE_NAME)
    {
        mediaplayer->setMedia(QUrl::fromLocalFile(itemText.split(" [")[0]));
        mediaplayer->play();
    }
}

void TabGridWidget::createWav(QString filename)
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
    int steps = 0;
    SliceMode_t sliceMode = SliceMode_t::GridMode;
    NormalizationMode_t normalizationMode = static_cast<NormalizationMode_t>(ui->dropNormalize->currentData().toInt());
    int fadeIn = ui->dropFadeIn->currentData().toInt();
    int fadeOut = ui->dropFadeOut->currentData().toInt();
    bool createOTFile = ui->chkCreateOTFile->isChecked();

    QVector<QString> sourceFiles;
    for (int i = 0; i < ui->listSlices->count(); i++)
    {
        QString itemText = ui->listSlices->item(i)->text();
        sourceFiles.append(itemText.split(" [")[0]);
    }

    QThread *workThread = new QThread;
    AudioFactory *af = new AudioFactory;
    af->setUISelections(sampleRate, bitRate, channels, sourceFiles, filename, loopSetting, stretchSetting, trigQuantSetting, gain, tempo, steps, sliceMode, normalizationMode, fadeIn, fadeOut, createOTFile, 1);
    af->moveToThread(workThread);
    connect(workThread, SIGNAL(started()), af, SLOT(generateFiles()));
    connect(af, SIGNAL(doneGenerating()), workThread, SLOT(quit()));
    connect(af, SIGNAL(doneGenerating()), this, SLOT(updateUIafterCreateDone()));
    connect(af, SIGNAL(fileProgress(int, int)), this, SLOT(updateProgressBar(int, int)));
    connect(workThread, SIGNAL(finished()), af, SLOT(deleteLater()));
    connect(workThread, SIGNAL(finished()), workThread, SLOT(deleteLater()));
    workThread->start();
}

void TabGridWidget::updateProgressBar(int currentSlice, int totalSlices)
{
    int percentage = 100*currentSlice/totalSlices;
    ui->progressBar->setValue(percentage);
}

void TabGridWidget::updateUIafterCreateDone()
{
    ui->progressBar->setVisible(false);
    ui->progressBar->setValue(0);
    ui->btnCreate->setEnabled(true);
}

void TabGridWidget::updateSliceCount()
{
    int count = ui->listSlices->count();
    ui->lblSliceCOunt->setText(QString::number(count));
    if (count < 1)
        ui->btnCreate->setEnabled(false);
    else
        ui->btnCreate->setEnabled(true);
}

void TabGridWidget::on_btnAddWav_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Select audio files", _defaultPathAudio, AudioUtil::audioFileFilter());
    addListItems(fileNames);
    updateSliceCount();
}

void TabGridWidget::on_playAudio_toggled()
{

    if (mediaplayer->state() == QMediaPlayer::PlayingState)
        mediaplayer->stop();
    else if (ui->listSlices->selectedItems().length() == 1)
        playAudio();
 }

void TabGridWidget::on_btnPlay_clicked()
{
    if (ui->listSlices->selectedItems().count() == 1)
        playAudio();
}

void TabGridWidget::on_listSlices_doubleClicked(const QModelIndex &index)
{
    playAudio();
}

void TabGridWidget::on_listSlices_itemSelectionChanged()
{
    ui->btnPlay->setEnabled(ui->listSlices->selectedItems().length() == 1);
}


void TabGridWidget::on_btnStop_clicked()
{
    mediaplayer->stop();
}

void TabGridWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void TabGridWidget::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    QStringList fileList;
    for (int i = 0; i < urls.count(); i++)
    {
        QString localFile = urls[i].toLocalFile();
        if (QFileInfo(localFile).isDir())
        {
            QStringList dirContents = QDir(localFile).entryList();
            for (int j = 0; j < dirContents.length(); j++)
            {
                 QString path = localFile + "/" + dirContents[j];
                if (QFileInfo(path).isFile() && AudioUtil::isAudioFileName(path))
                    fileList.append(path);
            }
        }
        else if (QFileInfo(localFile).isFile() && AudioUtil::isAudioFileName(localFile))
        {
            fileList.append(localFile);
        }
    }
    if (fileList.count() > 0)
    {
        addListItems(fileList);
        updateSliceCount();
    }
}

void TabGridWidget::on_btnRemove_clicked()
{
    while (ui->listSlices->selectedItems().count() > 0)
        delete ui->listSlices->selectedItems()[0];
    updateSliceCount();
}

void TabGridWidget::on_btnCreate_clicked()
{
    if (ui->listSlices->count() > 0)
    {
        QString destinationFile = QFileDialog::getSaveFileName(this, "Save as...", _defaultPathOutput, "Wave file (*.wav)");
        if (!destinationFile.isEmpty())
            createWav(destinationFile);
    }
}

void TabGridWidget::addListItems(const QStringList files)
{
    for (int i = 0; i < files.length(); i++)
    {
        ui->listSlices->addItem(files[i] + AudioFactory::getFormatString(files[i]));
    }
}

void TabGridWidget::on_sliderGain_valueChanged(int value)
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

void TabGridWidget::on_sliderBPM_valueChanged(int value)
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

void TabGridWidget::on_btnAddSilence_clicked()
{
    ui->listSlices->addItem(SILENT_SLICE_NAME);
    updateSliceCount();
}

void TabGridWidget::on_txtBPMValue_textChanged()
{
    QString txtValue = ui->txtBPMValue->toPlainText();
    int sliderValue = (int)(txtValue.toFloat() * 4.0 + 0.5);
    if (sliderValue != ui->sliderBPM->value() && sliderValue >= 30*4 && sliderValue <= 300*4)
        ui->sliderBPM->setValue(sliderValue);
}

void TabGridWidget::on_txtGainValue_textChanged()
{
    QString txtValue = ui->txtGainValue->toPlainText();
    int sliderValue = (int)(txtValue.toFloat() * 2.0);
    if (sliderValue != ui->sliderGain->value() && sliderValue >= -24*2 && sliderValue <= 24*2)
        ui->sliderGain->setValue(sliderValue);
}

void TabGridWidget::reset()
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
    ui->dropFadeIn->setCurrentIndex(0);
    ui->dropFadeOut->setCurrentIndex(0);
    on_sliderGain_valueChanged(0);
    on_sliderBPM_valueChanged(500);
    enableDisableOTSection(false);
}

void TabGridWidget::configure(ProjectSettings &settings)
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
    ui->dropFadeIn->setCurrentIndex(settings.fadein);
    ui->dropFadeOut->setCurrentIndex(settings.fadeout);

    if (ui->listSlices->count() > 0 && ui->listSlices->count() < 65)
        ui->btnCreate->setEnabled(true);

    ui->chkCreateOTFile->setChecked(settings.createOTFile);

    enableDisableOTSection(settings.createOTFile);
    updateSliceCount();
}

void TabGridWidget::updateCurrentSettings(ProjectSettings &settings)
{
    settings.modeName = settings.ModeName_Grid;
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
    settings.fadein = ui->dropFadeIn->currentIndex();
    settings.fadeout = ui->dropFadeOut->currentIndex();
    settings.createOTFile = ui->chkCreateOTFile->isChecked();

    for (int i = 0; i < ui->listSlices->count(); i++)
        settings.samplePaths.append(ui->listSlices->item(i)->text());
}

void TabGridWidget::on_chkCreateOTFile_stateChanged(int value)
{
    bool enable = value > 0;
    enableDisableOTSection(enable);

}

void TabGridWidget::enableDisableOTSection(bool enable)
{
    ui->dropLoop->setEnabled(enable);
    ui->dropQuant->setEnabled(enable);
    ui->dropStretch->setEnabled(enable);
    ui->sliderBPM->setEnabled(enable);
    ui->sliderGain->setEnabled(enable);
    ui->txtBPMValue->setEnabled(enable);
    ui->txtGainValue->setEnabled(enable);
}
