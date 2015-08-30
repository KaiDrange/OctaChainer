#include "audiofactory.h"

void AudioFactory::generateFiles()
{
    if (this->sliceMode == SliceMode_t::NormalMode)
        createOutput_NormalMode();
    else if (this->sliceMode == SliceMode_t::GridMode)
        createOutput_GridMode();
    else
        createOutput_StepsMode();

    emit doneGenerating();
}

void AudioFactory::setUISelections(const int sampleRate,
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
                     const SliceMode_t sliceMode)
{
    this->sampleRate = sampleRate;
    this->bitRate = bitRate;
    this->channels = channels;
    this->sourceFiles = sourceFiles;
    this->outFileName = outFileName;
    this->loopSetting = loopSetting;
    this->stretchSetting = stretchSetting;
    this->trigQuantSetting = trigQuantSetting;
    this->gain = gain;
    this->tempo = tempo;
    this->steps = steps;
    this->sliceMode = sliceMode;
}


void AudioFactory::createOutput_NormalMode()
{
    SF_INFO sfinfo;
    sfinfo.channels = channels;
    sfinfo.samplerate = sampleRate;
    if (bitRate == 16)
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    else
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;

    QString otName = outFileName;
    otName.chop(4);
    otName += ".ot";
    otWriter = new OTWriter(otName, sampleRate, loopSetting, stretchSetting, trigQuantSetting, gain, tempo);

    QByteArray outNameByteArray = outFileName.toLocal8Bit();
    SndfileHandle outFile = SndfileHandle (outNameByteArray.data(), SFM_WRITE, sfinfo.format, sfinfo.channels, sfinfo.samplerate) ;
    sf_count_t totalWrittenFrames = 0;

    if (sourceFiles.count() == 0 || sourceFiles.count() > 64)
        return;

    for (int i = 0; i < sourceFiles.count(); i++)
    {
        emit fileProgress(i, sourceFiles.count());
        bool convertToMono = false;
        bool convertToStereo = false;

        QString inName = sourceFiles[i];

        QByteArray ba = inName.toLocal8Bit();
        SndfileHandle inFile = SndfileHandle(ba.data());
        memset(buffer, 0, sizeof(buffer));
        if (inFile.channels() > outFile.channels())
            convertToMono = true;
        else if (inFile.channels() < outFile.channels())
            convertToStereo = true;

        sf_count_t readFrames = 0;
        sf_count_t sliceStartPoint = totalWrittenFrames;
        if (sourceFiles[i] == SILENT_SLICE_NAME) // add 1 sec silence
        {
            sf_count_t writtenFrames = 0;
            memset(buffer, 0, sizeof(buffer));
            while (writtenFrames < sampleRate*outFile.channels())
            {
                if (sampleRate*outFile.channels() - writtenFrames > BUFFER_LENGTH)
                    writtenFrames += outFile.write(buffer, BUFFER_LENGTH);
                else
                    writtenFrames += outFile.write(buffer, sampleRate*outFile.channels() - writtenFrames);
            }
            totalWrittenFrames += writtenFrames;
        }
        else
        {
            while ((readFrames = inFile.read(buffer, BUFFER_LENGTH)) > 0)
            {
                if (convertToMono)
                {
                    for (int j = 0; j < readFrames; j+=2)
                    {
                        int monoFrame = (buffer[j]+buffer[j+1])*0.5;
                        totalWrittenFrames += outFile.write(&monoFrame, 1);
                    }
                }
                else if (convertToStereo)
                {
                    for (int j = 0; j < readFrames; j++)
                    {
                        int stereoFrames[2] = { buffer[j], buffer[j] };
                        totalWrittenFrames += outFile.write(stereoFrames, 2);
                    }
                }
                else
                    totalWrittenFrames += outFile.write(buffer, readFrames);
            }
        }
        otWriter->addSlice((uint32_t)(sliceStartPoint/sfinfo.channels), (uint32_t)(totalWrittenFrames/sfinfo.channels));
    }
    otWriter->write((uint32_t)(totalWrittenFrames/sfinfo.channels));

    delete otWriter;
}

void AudioFactory::createOutput_GridMode()
{
    if (sourceFiles.count() == 0)
        return;
    bool atLeastOneNoneSilent = false;
    for (int i = 0; i < sourceFiles.count(); i++)
    {
        if (sourceFiles[i] != SILENT_SLICE_NAME)
        {
            atLeastOneNoneSilent = true;
            break;
        }
    }
    if (!atLeastOneNoneSilent)
        return;


    SF_INFO sfinfo;
    sfinfo.channels = channels;
    sfinfo.samplerate = sampleRate;
    if (bitRate == 16)
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    else
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;

    sf_count_t sliceSize = 0;
    for (int i = 0; i < sourceFiles.count(); i++)
    {
        if (sourceFiles[i] != SILENT_SLICE_NAME)
        {
            QString inName = sourceFiles[i];
            QByteArray ba = inName.toLocal8Bit();
            SndfileHandle inFile = SndfileHandle(ba.data());
            if (inFile.frames() > sliceSize)
                sliceSize = inFile.frames();
        }
    }
    sliceSize *= sfinfo.channels;

    QByteArray outNameByteArray = outFileName.toLocal8Bit();
    SndfileHandle outFile = SndfileHandle (outNameByteArray.data(), SFM_WRITE, sfinfo.format, sfinfo.channels, sfinfo.samplerate);

    for (int i = 0; i < sourceFiles.count(); i++)
    {
        sf_count_t writtenSliceSize = 0;

        if (sourceFiles[i] != SILENT_SLICE_NAME)
        {
            emit fileProgress(i, sourceFiles.count());

            bool convertToMono = false;
            bool convertToStereo = false;

            QString inName = sourceFiles[i];

            QByteArray ba = inName.toLocal8Bit();
            SndfileHandle inFile = SndfileHandle(ba.data());
            memset(buffer, 0, sizeof(buffer));
            if (inFile.channels() > outFile.channels())
                convertToMono = true;
            else if (inFile.channels() < outFile.channels())
                convertToStereo = true;

            sf_count_t readFrames = 0;
            while ((readFrames = inFile.read(buffer, BUFFER_LENGTH)) > 0)
            {
                if (convertToMono)
                {
                    for (int j = 0; j < readFrames; j+=2)
                    {
                        int monoFrame = (buffer[j]+buffer[j+1])*0.5;
                        writtenSliceSize += outFile.write(&monoFrame, 1);
                    }
                }
                else if (convertToStereo)
                {
                    for (int j = 0; j < readFrames; j++)
                    {
                        int stereoFrames[2] = { buffer[j], buffer[j] };
                        writtenSliceSize += outFile.write(stereoFrames, 2);
                    }
                }
                else
                    writtenSliceSize += outFile.write(buffer, readFrames);
            }
        }

        memset(buffer, 0, sizeof(buffer));
        while (writtenSliceSize < sliceSize)
        {
            if (sliceSize - writtenSliceSize > BUFFER_LENGTH)
                writtenSliceSize += outFile.write(buffer, BUFFER_LENGTH);
            else
                writtenSliceSize += outFile.write(buffer, sliceSize - writtenSliceSize);
        }
    }
}

void AudioFactory::createOutput_StepsMode()
{
    SF_INFO sfinfo;
    sfinfo.channels = channels;
    sfinfo.samplerate = sampleRate;
    if (bitRate == 16)
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    else
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;


    QByteArray outNameByteArray = outFileName.toLocal8Bit();
    SndfileHandle outFile = SndfileHandle (outNameByteArray.data(), SFM_WRITE, sfinfo.format, sfinfo.channels, sfinfo.samplerate) ;
    sf_count_t totalWrittenFrames = 0;

    if (sourceFiles.count() == 0 || sourceFiles.count() > 64)
        return;

    for (int i = 0; i < sourceFiles.count(); i++)
    {
        emit fileProgress(i, sourceFiles.count());

        bool convertToMono = false;
        bool convertToStereo = false;

        QString inName = sourceFiles[i];

        QByteArray ba = inName.toLocal8Bit();
        SndfileHandle inFile = SndfileHandle(ba.data());
        memset(buffer, 0, sizeof(buffer));
        if (inFile.channels() > outFile.channels())
            convertToMono = true;
        else if (inFile.channels() < outFile.channels())
            convertToStereo = true;

        sf_count_t readFrames = 0;
        if (sourceFiles[i] == SILENT_SLICE_NAME) // add 1 sec silence
        {
            sf_count_t writtenFrames = 0;
            memset(buffer, 0, sizeof(buffer));
            while (writtenFrames < sampleRate*outFile.channels())
            {
                if (sampleRate*outFile.channels() - writtenFrames > BUFFER_LENGTH)
                    writtenFrames += outFile.write(buffer, BUFFER_LENGTH);
                else
                    writtenFrames += outFile.write(buffer, sampleRate*outFile.channels() - writtenFrames);
            }
            totalWrittenFrames += writtenFrames;
        }
        else
        {
            while ((readFrames = inFile.read(buffer, BUFFER_LENGTH)) > 0)
            {
                if (convertToMono)
                {
                    for (int j = 0; j < readFrames; j+=2)
                    {
                        int monoFrame = (buffer[j]+buffer[j+1])*0.5;
                        totalWrittenFrames += outFile.write(&monoFrame, 1);
                    }
                }
                else if (convertToStereo)
                {
                    for (int j = 0; j < readFrames; j++)
                    {
                        int stereoFrames[2] = { buffer[j], buffer[j] };
                        totalWrittenFrames += outFile.write(stereoFrames, 2);
                    }
                }
                else
                    totalWrittenFrames += outFile.write(buffer, readFrames);
            }
        }
    }

    QString otName = outFileName;
    otName.chop(4);
    otName += ".ot";

    sf_count_t sliceSize = sampleRate*60*steps/tempo;
    sliceSize *= sfinfo.channels;

    otWriter = new OTWriter(otName, sampleRate, loopSetting, stretchSetting, trigQuantSetting, gain, tempo);
    int sliceCount = 0;
    while ((sliceCount+1) * sliceSize <= totalWrittenFrames && sliceCount < 64)
    {
        otWriter->addSlice((uint32_t)(sliceCount*sliceSize)/sfinfo.channels, (uint32_t)((sliceCount+1)*sliceSize)/sfinfo.channels);
        sliceCount++;
    }
    if (sliceCount < 64 && sliceCount*sliceSize < totalWrittenFrames)
        otWriter->addSlice((uint32_t)(sliceCount*sliceSize)/sfinfo.channels, (uint32_t)totalWrittenFrames/sfinfo.channels);
    otWriter->write((uint32_t)(totalWrittenFrames/sfinfo.channels));

    delete otWriter;
}


QString AudioFactory::getFormatString(const QString &file)
{
    QByteArray ba = file.toLocal8Bit();
    SndfileHandle inFile = SndfileHandle(ba.data());

    int sampleRate = inFile.samplerate();
    int channels = inFile.channels();
    int bitRate = -1;
    if ((inFile.format()&SF_FORMAT_PCM_S8) == SF_FORMAT_PCM_S8)
        bitRate = 8;
    if ((inFile.format()&SF_FORMAT_PCM_16) == SF_FORMAT_PCM_16)
        bitRate = 16;
    if ((inFile.format()&SF_FORMAT_PCM_24) == SF_FORMAT_PCM_24)
        bitRate = 24;
    if ((inFile.format()&SF_FORMAT_PCM_32))
        bitRate = 32;

    QString channelLetter = channels == 1 ? "M " : "S ";
    QString bitRateString = bitRate == -1 ? "?" : QString::number(bitRate);
    QString formatString = " [" + channelLetter;
    formatString += bitRateString + " " + QString::number(sampleRate) + "]";
    return formatString;
}
