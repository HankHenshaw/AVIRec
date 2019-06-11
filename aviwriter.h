#ifndef AVIWRITER_H
#define AVIWRITER_H

#include <QByteArray>
#include <QVector>
#include <QFile>
#include <QDebug>
#include <QPair>
#include <QIODevice>
#include <QPixmap>
#include <QImage>
#include <QTime>

struct Chunk
{
    unsigned int fourCC;
    unsigned int size;
};

struct List
{
    unsigned int list;
    unsigned int size;
    unsigned int fourCC;
};

struct AviMainHeader
{
    unsigned int microSecPerFrame;
    unsigned int maxBytesPerSec;
    unsigned int paddingGranularity;
    unsigned int flags;
    unsigned int totalFrames;
    unsigned int initialFrames;
    unsigned int streams;
    unsigned int suggestedBufferSize;
    unsigned int width;
    unsigned int height;
    unsigned int reserved[4];
};

struct AviStreamHeader
{
    unsigned int fccType;
    unsigned int fccHandler;
    unsigned int flags;
    unsigned short priority;
    unsigned short language;
    unsigned int initialFrames;
    unsigned int scale;
    unsigned int rate;
    unsigned int start;
    unsigned int length;
    unsigned int suggestedBufferSize;
    unsigned int quality;
    unsigned int sampleSize;
};

struct AviStreamFormat
{
    unsigned int size;
    unsigned int width;
    unsigned int height;
    unsigned short planes;
    unsigned short bitCount;
    unsigned int compression;
    unsigned int sizeImage;
    unsigned int xPelsPerMeter;
    unsigned int yPelsPerMeter;
    unsigned int clrUsed;
    unsigned int clrImportant;
};

struct AviIndex
{
    unsigned int ckid;
    unsigned int flags;
    unsigned int chunkOffset;
    unsigned int chunkLength;
};

class AVIWriter
{
public:
    AVIWriter();
    void start(unsigned int fps, unsigned int width, unsigned int height, const char* codec = "MJPG");
    void start(unsigned int width, unsigned int height, QString outputDirectory, int fps = 0, const char* codec = "MJPG");
    void addFrame(QImage *pix, int quality = -1);
    void stop();
    void stop(int time, int numberOfFrames);
private:
    List m_aviList;
    List m_hdrlList;
    Chunk m_aviChunk;
    AviMainHeader m_mainHeader;
    List m_strlList;
    Chunk m_strhChunk;
    AviStreamHeader m_aviStream;
    Chunk m_strfChunk;
    AviStreamFormat m_streamFormat;
    Chunk m_junk;
    List m_moviList;
    Chunk m_indexChunk;
    AviIndex m_index;
    QVector<unsigned int> m_framesSizes;
    QFile m_file;
    qint64 m_pos;
    qint64 m_pos2;

    qint64 m_microSecPerFramePos;
    qint64 m_ratePos;
    bool m_isFpsSet;
};

#endif // AVIWRITER_H
