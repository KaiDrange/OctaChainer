#include <QDataStream>
#include <QFile>
#include "otwriter.h"

const unsigned char header_bytes[] = {0x46,0x4F,0x52,0x4D,0x00,0x00,0x00,0x00,0x44,0x50,0x53,0x31,0x53,0x4D,0x50,0x41};
const unsigned char unknown_bytes[] = {0x00,0x00,0x00,0x00,0x00,0x02,0x00};

OTWriter::OTWriter(const QString fileName, const int sampleRate, const Loop_t loopSetting, const Stretch_t strechSetting, const TrigQuant_t trigQuantSetting, int gain, int tempo)
{
    this->fileName = fileName;
    std::fill_n((char*)&otData, sizeof(otData), 0);
    memcpy(&otData, header_bytes, 16);
    memcpy(&otData.unknown, unknown_bytes, 7);

    this->tempo = tempo;
    this->sampleRate = sampleRate;
    this->gain = gain;
    this->loopSetting = loopSetting;
    this->stretchSetting = strechSetting;
    this->trigQuantSetting = trigQuantSetting;
}

void OTWriter::updateData()
{
    otData.tempo = SYSTEM_USE_LITTLE_ENDIAN ? _byteswap_ulong(tempo*6) : tempo*6;
    // calculate bars from BPM, sampleRate and audioLength and round off to closest .25
    int bars = ((tempo*totalSampleCount)/(sampleRate*60.0*4)) + 0.5;
    bars *= 25;
    otData.trimLen = SYSTEM_USE_LITTLE_ENDIAN ? _byteswap_ulong(bars) : bars;
    otData.loopLen = SYSTEM_USE_LITTLE_ENDIAN ? _byteswap_ulong(bars) : bars;
    otData.loop = SYSTEM_USE_LITTLE_ENDIAN ? _byteswap_ulong(static_cast<uint32_t>(loopSetting)) : static_cast<uint32_t>(loopSetting);
    otData.stretch = SYSTEM_USE_LITTLE_ENDIAN ? _byteswap_ulong(static_cast<uint32_t>(stretchSetting)) : static_cast<uint32_t>(stretchSetting);
    otData.quantize = trigQuantSetting;
    otData.gain = SYSTEM_USE_LITTLE_ENDIAN ? _byteswap_ushort(gain + 48) : gain + 48;

    if (tempSlices.count() > 1)
    {
        for (int i = 0; i < tempSlices.count(); i++)
        {
            otData.slices[i].startPoint = SYSTEM_USE_LITTLE_ENDIAN ? _byteswap_ulong(tempSlices[i].startPoint) : tempSlices[i].startPoint;
            otData.slices[i].endPoint = SYSTEM_USE_LITTLE_ENDIAN ? _byteswap_ulong(tempSlices[i].endPoint) : tempSlices[i].endPoint;
            otData.slices[i].loopPoint = SYSTEM_USE_LITTLE_ENDIAN ? _byteswap_ulong(tempSlices[i].loopPoint) : tempSlices[i].loopPoint;
        }
        otData.sliceCount = SYSTEM_USE_LITTLE_ENDIAN ? _byteswap_ulong(tempSlices.count()) : tempSlices.count();
    }
    else
        otData.sliceCount = 0;
    otData.trimEnd = SYSTEM_USE_LITTLE_ENDIAN ? _byteswap_ulong(totalSampleCount) : totalSampleCount;
    setChecksum();
}

void OTWriter::write(uint32_t totalSamples)
{
    this->totalSampleCount = totalSamples;
    updateData();
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QDataStream outStream(&file);

    outStream.writeRawData((char*)&otData.header, sizeof(otData.header));
    outStream.writeRawData((char*)&otData.unknown, sizeof(otData.unknown));
    outStream.writeRawData((char*)&otData.tempo, sizeof(otData.tempo));
    outStream.writeRawData((char*)&otData.trimLen, sizeof(otData.trimLen));
    outStream.writeRawData((char*)&otData.loopLen, sizeof(otData.loopLen));
    outStream.writeRawData((char*)&otData.stretch, sizeof(otData.stretch));
    outStream.writeRawData((char*)&otData.loop, sizeof(otData.loop));
    outStream.writeRawData((char*)&otData.gain, sizeof(otData.gain));
    outStream.writeRawData((char*)&otData.quantize, sizeof(otData.quantize));
    outStream.writeRawData((char*)&otData.trimStart, sizeof(otData.trimStart));
    outStream.writeRawData((char*)&otData.trimEnd, sizeof(otData.trimEnd));
    outStream.writeRawData((char*)&otData.loopPoint, sizeof(otData.loopPoint));
    outStream.writeRawData((char*)&otData.slices, sizeof(Slice)*64);
    outStream.writeRawData((char*)&otData.sliceCount, sizeof(otData.sliceCount));
    outStream.writeRawData((char*)&otData.checkSum, sizeof(otData.checkSum));
    file.close();
}

void OTWriter::setChecksum()
{
    uint16_t value = 0;
    unsigned char* bytePos = reinterpret_cast<unsigned char*>(&otData);
    for (size_t i = 16; i < sizeof(otData) - 2; i++)
        value += bytePos[i];

    otData.checkSum = SYSTEM_USE_LITTLE_ENDIAN ? _byteswap_ushort(value) : value;
}

void OTWriter::addSlice(uint32_t start, uint32_t end)
{
    Slice slice;
    slice.loopPoint = 0xFFFFFFFF;
    slice.startPoint = start;
    slice.endPoint = end;
    this->tempSlices.append(slice);
}
