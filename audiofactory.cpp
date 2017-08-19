#include "audiofactory.h"

void AudioFactory::generateFiles()
{
    if (this->sliceMode == SliceMode_t::NormalMode)
        createOutput_NormalMode();
    else if (this->sliceMode == SliceMode_t::GridMode)
        createOutput_GridMode();
    else if (this->sliceMode == SliceMode_t::StepsMode)
        createOutput_StepsMode();
    else
        createOutput_MegabreakMode();

    if (this->normalizationMode == NormalizationMode_t::Chain) {
        if (this->sliceMode == SliceMode_t::MegabreakMode) {
            for (int i = 0; i < this->megabreakFiles; i++) {
                QString otName = outFileName;
                otName.chop(4);
                otName += i < 10 ? "0" + QVariant(i).toString() : QVariant(i).toString();
                otName += ".wav";
                normalizeChain(otName);
            }
        }
        else
            normalizeChain(this->outFileName);
    }

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
                     const SliceMode_t sliceMode,
                     const NormalizationMode_t normalizationMode,
                     const int fadeIn,
                     const int fadeOut,
                     const bool createOTFile,
                     const int megabreakFiles)
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
    this->normalizationMode = normalizationMode;
    this->fadeIn_ms = fadeIn;
    this->fadeOut_ms = fadeOut;
    this->createOTFile = createOTFile;
    this->megabreakFiles = megabreakFiles;
}

void AudioFactory::setStepsModeUISelections(const QString &sourcefile,
                    const Loop_t loopSetting,
                    const Stretch_t stretchSetting,
                    const TrigQuant_t trigQuantSetting,
                    const int gain,
                    const int tempo,
                    const int steps,
                    const bool includeTail)
{
    QByteArray ba = sourcefile.toLocal8Bit();
    SndfileHandle file = SndfileHandle(ba.data());

    this->sampleRate = file.samplerate();
    if ((file.format()&SF_FORMAT_PCM_S8) == SF_FORMAT_PCM_S8)
        this->bitRate = 8;
    if ((file.format()&SF_FORMAT_PCM_16) == SF_FORMAT_PCM_16)
        this->bitRate = 16;
    if ((file.format()&SF_FORMAT_PCM_24) == SF_FORMAT_PCM_24)
        this->bitRate = 24;
    if ((file.format()&SF_FORMAT_PCM_32))
        this->bitRate = 32;
    this->channels = file.channels();
    this->sourceFiles.append(sourcefile);
    this->outFileName = sourcefile;
    this->loopSetting = loopSetting;
    this->stretchSetting = stretchSetting;
    this->trigQuantSetting = trigQuantSetting;
    this->gain = gain;
    this->tempo = tempo;
    this->steps = steps;
    this->sliceMode = SliceMode_t::StepsMode;
    this->normalizationMode = NormalizationMode_t::None;
    this->createOTFile = true;
    this->includeTail = includeTail;
}

void AudioFactory::createOutput_NormalMode()
{
    if (sourceFiles.count() == 0 || sourceFiles.count() > 64)
        return;

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

    int fadeInCurveLength = sfinfo.samplerate*fadeIn_ms/1000;
    int fadeOutCurveLength = sfinfo.samplerate*fadeOut_ms/1000;
    float fadeInCurve[fadeInCurveLength];
    float fadeOutCurve[fadeOutCurveLength];
    createFadeCurves(fadeInCurveLength, fadeOutCurveLength, fadeInCurve, fadeOutCurve);

    QByteArray outNameByteArray = outFileName.toLocal8Bit();
    SndfileHandle outFile = SndfileHandle (outNameByteArray.data(), SFM_WRITE, sfinfo.format, sfinfo.channels, sfinfo.samplerate) ;
    sf_count_t totalWrittenFrames = 0;

    for (int i = 0; i < sourceFiles.count(); i++)
    {
        emit fileProgress(i, sourceFiles.count());
        bool convertToMono = false;
        bool convertToStereo = false;

        QString inName = sourceFiles[i];

        QByteArray ba = inName.toLocal8Bit();
        SndfileHandle inFile = SndfileHandle(ba.data());
        if (inFile.channels() > outFile.channels())
            convertToMono = true;
        else if (inFile.channels() < outFile.channels())
            convertToStereo = true;

        double normalizationFactor = findNormalizationFactor(&inFile);
        memset(buffer, 0, sizeof(buffer));

        sf_count_t readFrames = 0;
        sf_count_t previousReadFramesTotal = 0;
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
                if (this->normalizationMode == NormalizationMode_t::Slice)
                {
                    for (int i = 0; i < BUFFER_LENGTH; i++)
                        buffer[i] = buffer[i]*normalizationFactor;
                }

                for (int i = previousReadFramesTotal; i < fadeInCurveLength * inFile.channels(); i++)
                {
                    if ((i - previousReadFramesTotal) < BUFFER_LENGTH)
                        buffer[i - previousReadFramesTotal] = buffer[i- previousReadFramesTotal]*fadeInCurve[i/inFile.channels()];
                }

                for (int i = (inFile.frames() - fadeOutCurveLength) * inFile.channels(); i < previousReadFramesTotal + readFrames; i++)
                {
                    if (i - previousReadFramesTotal < BUFFER_LENGTH)
                    {
                        int curveIndex = inFile.frames() - i/inFile.channels() - 1;
                        buffer[i - previousReadFramesTotal] = buffer[i - previousReadFramesTotal]*fadeOutCurve[curveIndex];
                    }
                }

                if (convertToMono)
                {
                    for (int j = 0; j < readFrames; j+=2)
                    {
                        int monoFrame = buffer[j]*0.5+buffer[j+1]*0.5;
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

                previousReadFramesTotal += readFrames;
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

    if (createOTFile)
    {
        QString otName = outFileName;
        otName.chop(4);
        otName += ".ot";
        otWriter = new OTWriter(otName, sampleRate, loopSetting, stretchSetting, trigQuantSetting, gain, tempo);
    }


    SF_INFO sfinfo;
    sfinfo.channels = channels;
    sfinfo.samplerate = sampleRate;
    if (bitRate == 16)
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    else
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;

    sf_count_t totalWrittenFrames = 0;
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

    int fadeInCurveLength = sfinfo.samplerate*fadeIn_ms/1000;
    int fadeOutCurveLength = sfinfo.samplerate*fadeOut_ms/1000;
    float fadeInCurve[fadeInCurveLength];
    float fadeOutCurve[fadeOutCurveLength];
    createFadeCurves(fadeInCurveLength, fadeOutCurveLength, fadeInCurve, fadeOutCurve);

    QByteArray outNameByteArray = outFileName.toLocal8Bit();
    SndfileHandle outFile = SndfileHandle (outNameByteArray.data(), SFM_WRITE, sfinfo.format, sfinfo.channels, sfinfo.samplerate);

    for (int i = 0; i < sourceFiles.count(); i++)
    {
        sf_count_t writtenSliceSize = 0;
        sf_count_t sliceStartPoint = totalWrittenFrames;

        if (sourceFiles[i] != SILENT_SLICE_NAME)
        {
            emit fileProgress(i, sourceFiles.count());

            bool convertToMono = false;
            bool convertToStereo = false;

            QString inName = sourceFiles[i];

            QByteArray ba = inName.toLocal8Bit();
            SndfileHandle inFile = SndfileHandle(ba.data());
            if (inFile.channels() > outFile.channels())
                convertToMono = true;
            else if (inFile.channels() < outFile.channels())
                convertToStereo = true;

            double normalizationFactor = findNormalizationFactor(&inFile);

            memset(buffer, 0, sizeof(buffer));
            sf_count_t readFrames = 0;
            sf_count_t previousReadFramesTotal = 0;

            while ((readFrames = inFile.read(buffer, BUFFER_LENGTH)) > 0)
            {
                if (this->normalizationMode == NormalizationMode_t::Slice)
                {
                    for (int i = 0; i < BUFFER_LENGTH; i++)
                        buffer[i] = buffer[i]*normalizationFactor;
                }

                for (int i = previousReadFramesTotal; i < fadeInCurveLength * inFile.channels(); i++)
                {
                    if ((i - previousReadFramesTotal) < BUFFER_LENGTH)
                        buffer[i - previousReadFramesTotal] = buffer[i- previousReadFramesTotal]*fadeInCurve[i/inFile.channels()];
                }

                for (int i = (inFile.frames() - fadeOutCurveLength) * inFile.channels(); i < previousReadFramesTotal + readFrames; i++)
                {
                    if (i - previousReadFramesTotal < BUFFER_LENGTH)
                    {
                        int curveIndex = inFile.frames() - i/inFile.channels() - 1;
                        buffer[i - previousReadFramesTotal] = buffer[i - previousReadFramesTotal]*fadeOutCurve[curveIndex];
                    }
                }


                if (convertToMono)
                {
                    for (int j = 0; j < readFrames; j+=2)
                    {
                        int monoFrame = buffer[j]*0.5+buffer[j+1]*0.5;
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

                previousReadFramesTotal += readFrames;
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

        totalWrittenFrames += writtenSliceSize;
        if (createOTFile)
            otWriter->addSlice((uint32_t)(sliceStartPoint/sfinfo.channels), (uint32_t)(totalWrittenFrames/sfinfo.channels));
    }
    if (createOTFile)
    {
        otWriter->write((uint32_t)(totalWrittenFrames/sfinfo.channels));
        delete otWriter;
    }
}

void AudioFactory::createOutput_StepsMode()
{
    if (sourceFiles.count() == 0)
        return;

    SF_INFO sfinfo;
    sfinfo.channels = channels;
    sfinfo.samplerate = sampleRate;
    if (bitRate == 16)
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    else
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;

    emit fileProgress(0, sourceFiles.count());

    QString otName = sourceFiles[0];
    otName.chop(4);
    otName += ".ot";

    int totalSlices = 0;
    bool addTailSlice = false;
    getStepsModeSliceCount(steps, tempo, includeTail, sourceFiles[0], totalSlices, addTailSlice);
    double sliceSize = sampleRate*60.0*steps/tempo;

    QByteArray ba = sourceFiles[0].toLocal8Bit();
    SndfileHandle audioFile = SndfileHandle(ba.data());

    otWriter = new OTWriter(otName, sampleRate, loopSetting, stretchSetting, trigQuantSetting, gain, tempo);

    int sliceCount = 0;
    while (sliceCount < totalSlices && sliceCount < 64)
    {
        otWriter->addSlice((uint32_t)(sliceCount*sliceSize), (uint32_t)((sliceCount+1)*sliceSize));
        sliceCount++;
    }
    if (addTailSlice && sliceCount < 64)
        otWriter->addSlice((uint32_t)(sliceCount*sliceSize), (uint32_t)audioFile.frames()/sfinfo.channels);

    otWriter->write((uint32_t)(audioFile.frames()));
    delete otWriter;
}

void AudioFactory::createOutput_MegabreakMode() {
    if (sourceFiles.count() == 0 || sourceFiles.count() > 64)
        return;

    SF_INFO sfinfo;
    sfinfo.channels = channels;
    sfinfo.samplerate = sampleRate;
    if (bitRate == 16)
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    else
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;

    OTWriter* otWriters[this->megabreakFiles];
    SndfileHandle outFiles[64];
    for (int i = 0; i < this->megabreakFiles; i++) {
        QString otName = outFileName;
        otName.chop(4);
        otName += i < 10 ? "0" + QVariant(i).toString() : QVariant(i).toString();
        otWriters[i] = new OTWriter(otName + ".ot", sampleRate, loopSetting, stretchSetting, trigQuantSetting, gain, tempo);
        otName += ".wav";
        QByteArray outNameByteArray = otName.toLocal8Bit();
        outFiles[i] = SndfileHandle (outNameByteArray.data(), SFM_WRITE, sfinfo.format, sfinfo.channels, sfinfo.samplerate) ;
    }

    int fadeInCurveLength = sfinfo.samplerate*fadeIn_ms/1000;
    int fadeOutCurveLength = sfinfo.samplerate*fadeOut_ms/1000;
    float fadeInCurve[fadeInCurveLength];
    float fadeOutCurve[fadeOutCurveLength];
    createFadeCurves(fadeInCurveLength, fadeOutCurveLength, fadeInCurve, fadeOutCurve);


    sf_count_t totalWrittenFrames = 0;
    for (int i = 0; i < sourceFiles.count(); i++)
    {
        emit fileProgress(i, sourceFiles.count());
        bool convertToMono = false;
        bool convertToStereo = false;

        QString inName = sourceFiles[i];

        QByteArray ba = inName.toLocal8Bit();
        SndfileHandle inFile = SndfileHandle(ba.data());
        if (inFile.channels() > outFiles[i].channels())
            convertToMono = true;
        else if (inFile.channels() < outFiles[i].channels())
            convertToStereo = true;

        double normalizationFactor = findNormalizationFactor(&inFile);
        memset(buffer, 0, sizeof(buffer));

        sf_count_t readFrames = 0;
        sf_count_t previousReadFramesTotal = 0;
        sf_count_t sliceStartPoint = totalWrittenFrames;
        int currentOutIndex = 0;
        sf_count_t sliceLength = inFile.frames()/this->megabreakFiles;

        while ((readFrames = inFile.read(buffer, BUFFER_LENGTH)) > 0)
        {
            if (this->normalizationMode == NormalizationMode_t::Slice)
            {
                for (int j = 0; j < BUFFER_LENGTH; j++)
                    buffer[j] = buffer[j]*normalizationFactor;
            }

//                for (int j = previousReadFramesTotal; j < fadeInCurveLength * inFile.channels(); j++)
//                {
//                    if ((j - previousReadFramesTotal) < BUFFER_LENGTH)
//                        buffer[j - previousReadFramesTotal] = buffer[j- previousReadFramesTotal]*fadeInCurve[j/inFile.channels()];
//                }

//                for (int j = (inFile.frames() - fadeOutCurveLength) * inFile.channels(); j < previousReadFramesTotal + readFrames; j++)
//                {
//                    if (j - previousReadFramesTotal < BUFFER_LENGTH)
//                    {
//                        int curveIndex = inFile.frames() - j/inFile.channels() - 1;
//                        buffer[j - previousReadFramesTotal] = buffer[j - previousReadFramesTotal]*fadeOutCurve[curveIndex];
//                    }
//                }

//            if (convertToMono)
//            {
//                for (int j = 0; j < readFrames; j+=2)
//                {
//                    int monoFrame = buffer[j]*0.5+buffer[j+1]*0.5;
//                    totalWrittenFrames += outFile.write(&monoFrame, 1);
//                }
//            }
//            else if (convertToStereo)
//            {
//                for (int j = 0; j < readFrames; j++)
//                {
//                    int stereoFrames[2] = { buffer[j], buffer[j] };
//                    totalWrittenFrames += outFile.write(stereoFrames, 2);
//                }
//            }
//            else {
                if (previousReadFramesTotal + readFrames > sliceLength*(currentOutIndex+1)) {
                    sf_count_t framesForNextSlice = previousReadFramesTotal + readFrames - sliceLength*(currentOutIndex+1);
                    totalWrittenFrames += outFiles[currentOutIndex].write(buffer, readFrames - framesForNextSlice);
                    currentOutIndex++;
                    totalWrittenFrames += outFiles[currentOutIndex].write(&buffer[readFrames - framesForNextSlice], framesForNextSlice);
                }
                else
                    totalWrittenFrames += outFiles[currentOutIndex].write(buffer, readFrames);
//            }
            previousReadFramesTotal += readFrames;
        }
//        otWriter->addSlice((uint32_t)(sliceStartPoint/sfinfo.channels), (uint32_t)(totalWrittenFrames/sfinfo.channels));
    }
//    otWriter->write((uint32_t)(totalWrittenFrames/sfinfo.channels));

    for (int i = 0; i < this->megabreakFiles; i++) {
        delete otWriters[i];
    }
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

void AudioFactory::getStepsModeSliceCount(const int stepsPerSlice, const int tempo, bool includeTail, const QString &file, int &sliceCount, bool &hasTail)
{
    QByteArray ba = file.toLocal8Bit();
    SndfileHandle audioFile = SndfileHandle(ba.data());

    double sliceSize = audioFile.samplerate()*60.0*stepsPerSlice/tempo;
    sliceCount = audioFile.frames()/sliceSize;
    hasTail = (audioFile.frames() - sliceSize * sliceCount) > 1 && includeTail;
}


double AudioFactory::findNormalizationFactor(SndfileHandle *file)
{
    sf_count_t readFrames = 0;
    int loudestValue = 0;
    memset(buffer, 0, sizeof(buffer));
    while ((readFrames = file->read(buffer, BUFFER_LENGTH)) > 0)
    {
        for (int i = 0; i < BUFFER_LENGTH; i++)
        {
            if (abs(buffer[i]) > loudestValue)
                loudestValue = abs(buffer[i]);
        }
    }
    file->seek(0, SEEK_SET);

    double factor = 0;
    if (loudestValue > 0)
        factor = (std::numeric_limits<int>::max() *1.0f)/loudestValue;
    return factor;
}

void AudioFactory::normalizeChain(const QString filename)
{
    QFile tempFile(filename);
    QString tempFileName = filename + "_temp";
    tempFile.rename(tempFileName);

    QByteArray inNameByteArray = tempFileName.toLocal8Bit();
    SndfileHandle inFile = SndfileHandle(inNameByteArray.data(), SFM_READ);

    QByteArray outNameByteArray = filename.toLocal8Bit();
    SndfileHandle outFile = SndfileHandle(outNameByteArray.data(), SFM_WRITE, inFile.format(), inFile.channels(), inFile.samplerate());
    sf_count_t readFrames = 0;
    double factor = this->findNormalizationFactor(&inFile);
    memset(buffer, 0, sizeof(buffer));
    while ((readFrames = inFile.read(buffer, BUFFER_LENGTH)) > 0)
    {
        for (int i = 0; i < BUFFER_LENGTH; i++)
            buffer[i] = buffer[i]*factor;
        outFile.write(buffer, readFrames);
    }
    tempFile.remove();
}

void AudioFactory::createFadeCurves(const int fadeInCurveLength, const int fadeOutCurveLength, float fadeInCurve[], float fadeOutCurve[])
{
    if (fadeInCurveLength > 0)
    {
        for (int i = 0; i < fadeInCurveLength; i++)
            fadeInCurve[i] = (i*1.0)/fadeInCurveLength;
    }

    if (fadeOutCurveLength > 0)
        for (int i = 0; i < fadeOutCurveLength; i++) {
            fadeOutCurve[i] = (i*1.0)/fadeOutCurveLength;
    }
}
