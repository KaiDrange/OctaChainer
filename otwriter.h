#ifndef OTWRITER_H
#define OTWRITER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <stdint.h>
#ifdef _WIN32
#include <intrin.h>
#else
#define _byteswap_ulong __builtin_bswap32
#define _byteswap_ushort __builtin_bswap16
#endif

#define OTFILESIZE 832
#define SYSTEM_USE_LITTLE_ENDIAN 1

enum Loop_t { NoLoop = 0, Loop = 1, PIPO = 2 };
enum Stretch_t { NoStrech = 0, Normal = 2, Beat = 3 };
enum TrigQuant_t { Direct = 0xFF, Pattern = 0,S_1=1,S_2=2,S_3=3,S_4=4,S_6=5,S_8=6,S_12=7,S_16=8,S_24=9,S_32=10,S_48=11,S_64=12,S_96=13,S_128=14,S_192=15,S_256=16};

#pragma pack(push)
#pragma pack(1)
struct Slice
{
    uint32_t startPoint;
    uint32_t endPoint;
    uint32_t loopPoint;
};

struct OTData {
    uint8_t header[16];  // 0x00
    uint8_t unknown[7];  // 0x10 (All blank except 0x15 = 2)
    uint32_t tempo;      // 0x17 (BPM*24)
    uint32_t trimLen;    // 0x1B (value*100)
    uint32_t loopLen;    // 0x1F (value*100)
    uint32_t stretch;    // 0x23 (0ff = 0, Normal = 2, Beat = 3)
    uint32_t loop;       // 0x27 (Off = 0, Normal = 1, PingPong = 2)
    uint16_t gain;       // 0x2B (0x30 = 0, 0x60 = 24 (max), 0x00 = -24 (min))
    uint8_t quantize;    // 0x2D (0x00 = Pattern length, 0xFF = Direct, 1-10 = 1,2,3,4,6,8,12,16,24,32,48,64,96,128,192,256)
    uint32_t trimStart;  // 0x2E
    uint32_t trimEnd;    // 0x32
    uint32_t loopPoint;  // 0x36
    Slice slices[64];    // 0x3A - 0x339
    uint32_t sliceCount; // 0x33A
    uint16_t checkSum;   // 0x33E
};
#pragma pack(pop)

class OTWriter {

public:
    OTWriter(const QString fileName, const int sampleRate, const Loop_t loopSetting, const Stretch_t strechSetting, const TrigQuant_t trigQuantSetting, int gain, int tempo);
    void write(uint32_t totalSamples);
    void addSlice(uint32_t start, uint32_t end);
private:
    void updateData();
    void setChecksum();

    uint32_t totalSampleCount;
    QString fileName;
    int sampleRate;
    int tempo;
    int gain;
    Loop_t loopSetting;
    Stretch_t stretchSetting;
    TrigQuant_t trigQuantSetting;
    QVector<Slice> tempSlices;
    OTData otData;
};


#endif // OTWRITER_H

