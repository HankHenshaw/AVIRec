#include "aviwriter.h"

AVIWriter::AVIWriter()
{
    /*Обнуляем*/
    m_aviList = {};
    m_hdrlList = {};
    m_aviChunk = {};
    m_mainHeader = {};
    m_strlList = {};
    m_strhChunk = {};
    m_aviStream = {};
    m_strfChunk = {};
    m_streamFormat = {};
    m_junk = {};
    m_moviList = {};
    m_framesSizes = {};
    m_indexChunk = {};
    m_index = {};
    m_pos = 0;
    m_pos2 = 0;
    /*Обнуляем*/

    /*Avi List*/
    m_aviList.list = 0x46464952; // RIFF
    m_aviList.fourCC = 0x20495641; //AVI
    /*Avi List*/

    /*hdrl List*/
    m_hdrlList.list = 0x5453494c; // LIST
    m_hdrlList.fourCC = 0x6c726468; //hdrl
    /*hdrl List*/

    /*Avi Chunk*/
    m_aviChunk.fourCC = 0x68697661; //avih
    m_aviChunk.size = sizeof(AviMainHeader);
    /*Avi Chunk*/

    //MainHeader

    /*strl List*/
    m_strlList.list = 0x5453494c; //LIST
    m_strlList.size = sizeof(AviStreamFormat) + sizeof(AviStreamHeader) + 28; // 28 = 4(LIST) + 4(Size of list chunk) + 4(strl) + 4(strh) + 4(size of strh chunk) + 4(strf) + 4(size of strf chunk) WARNING Possible wrong value
    m_strlList.fourCC = 0x6c727473; // strl
    /*strl List*/

    /*strh Chunk*/
    m_strhChunk.fourCC = 0x68727473; // strh
    m_strhChunk.size = sizeof(AviStreamHeader) + 8; // 4 bytes for strh and 4 bytes for size of this chunk
    /*strh Chunk*/

    //Stream Header

    /*strf Chunk*/
    m_strfChunk.fourCC = 0x66727473; //strf
    m_strfChunk.size = sizeof(AviStreamFormat);
    /*strf Chunk*/

    //Stream Format

    /*JUNK Chunk*/
    m_junk.fourCC = 0x4b4e554a; // JUNK
    //m_junk.size //размер блока Junk
    /*JUNK Chunk*/

    /*Movi List*/
    m_moviList.list = 0x5453494c; //LIST
    m_moviList.fourCC = 0x69766F6D; //movi
    /*Movi List*/

    /*Index Chunk*/
    m_indexChunk.fourCC = 0x31786469; //idx1
    /*Index Chunk*/
}

void AVIWriter::start(unsigned int fps, unsigned int width, unsigned int height, const char *codec)
{
    if(!fps)
        qDebug() << "fps can't be null";

    if(!(width && height))
        qDebug() << "frame size can't be null";

    /*Main Header*/
    m_mainHeader.microSecPerFrame = 1000000 / fps;
    m_mainHeader.maxBytesPerSec = width*height*4;
    m_mainHeader.paddingGranularity = 0;
    m_mainHeader.flags = 0x1001;
    m_mainHeader.totalFrames = 0; // Пока 0, т.к. еще не добавлено ни одного кадра
    m_mainHeader.initialFrames = 0;
    m_mainHeader.streams = 1; // Пока 1, потом в зависимости от кол-ва потоков
    m_mainHeader.suggestedBufferSize = width*height*4;
    m_mainHeader.width = width;
    m_mainHeader.height = height;
    /*Main Header*/

    /*Avi stream header*/
    m_aviStream.fccType = 0x73646976; // vids
    m_aviStream.fccHandler = static_cast<unsigned int>(reinterpret_cast<size_t>(codec));
    m_aviStream.flags = 0;
    m_aviStream.priority = 0;
    m_aviStream.language = 0;
    m_aviStream.initialFrames = 0;
    m_aviStream.scale = 1;
    m_aviStream.rate = fps;
    m_aviStream.start = 0;
    m_aviStream.length = 0; // пока 0, т.к. еще не добавлено ни одного кадра
    m_aviStream.suggestedBufferSize = width*height*4;
    m_aviStream.quality = -1; //WARNING: Possible problem
    m_aviStream.sampleSize = width*height*4;
    /*Avi stream header*/

    /*Avi stream format*/
    m_streamFormat.size = sizeof(AviStreamFormat);
    m_streamFormat.width = width;
    m_streamFormat.height = height;
    m_streamFormat.planes = 1;
    m_streamFormat.bitCount = 32;
    m_streamFormat.compression = m_aviStream.fccHandler;
    m_streamFormat.sizeImage = width*height*4;
    m_streamFormat.xPelsPerMeter = 0;
    m_streamFormat.yPelsPerMeter = 0;
    m_streamFormat.clrUsed = 0;
    m_streamFormat.clrImportant = 0;
    /*Avi stream format*/

    /*Записываем в файл*/
    m_file.setFileName("test.avi");
    if(!m_file.open(QIODevice::WriteOnly))
        qDebug() << "Error while opening file";

    if(m_file.write(reinterpret_cast<char*>(&m_aviList.list), 4) == -1)
        qDebug() << "Error while writing m_aviList.list";

    //Должно быть четыре 0, размер всего файла без последней секции junk
    if(m_file.write(reinterpret_cast<char*>(&m_aviList.size), 4) == -1)
        qDebug() << "Error while writing m_aviList.size";

    if(m_file.write(reinterpret_cast<char*>(&m_aviList.fourCC), 4) == -1)
        qDebug() << "Error while writing m_aviList.fourCC";

    if(m_file.write(reinterpret_cast<char*>(&m_hdrlList.list), 4) == -1)
        qDebug() << "Error while writing m_hdrlList.list";

    //Размер данных этого листа, т.е всех подзаголовков + секция junk, пока четыре 0
    if(m_file.write(reinterpret_cast<char*>(&m_hdrlList.size), 4) == -1)
        qDebug() << "Error while writing m_hdrlList.size";

    if(m_file.write(reinterpret_cast<char*>(&m_hdrlList.fourCC), 4) == -1)
        qDebug() << "Error while writing &m_hdrlList.fourCC";

    if(m_file.write(reinterpret_cast<char*>(&m_aviChunk.fourCC), 4) == -1)
        qDebug() << "Error while writing m_aviChunk.fourCC";

    if(m_file.write(reinterpret_cast<char*>(&m_aviChunk.size), 4) == -1)
        qDebug() << "Error while writing m_aviChunk.size";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.microSecPerFrame), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.microSecPerFrame";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.maxBytesPerSec), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.maxBytesPerSec";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.paddingGranularity), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.paddingGranularity";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.flags), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.flags";

    // Пока 0, т.к. нету кадров
    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.totalFrames), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.totalFrames";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.initialFrames), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.initialFrames";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.streams), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.streams";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.suggestedBufferSize), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.suggestedBufferSize";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.width), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.width";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.height), 4) == -1)
        qDebug() << "Error while writing &m_mainHeader.height";

    for(int i = 0; i < 4; ++i)
        if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.reserved[i]), 4) == -1)
            qDebug() << "Error while writing m_mainHeader.reserved" << i;

    if(m_file.write(reinterpret_cast<char*>(&m_strlList.list), 4) == -1)
        qDebug() << "Error while writing m_strlList.list";

    if(m_file.write(reinterpret_cast<char*>(&m_strlList.size), 4) == -1)
        qDebug() << "Error while writing m_strlList.size";

    if(m_file.write(reinterpret_cast<char*>(&m_strlList.fourCC), 4) == -1)
        qDebug() << "Error while writing m_strlList.fourCC";

    if(m_file.write(reinterpret_cast<char*>(&m_strhChunk.fourCC), 4) == -1)
        qDebug() << "Error while writing m_strhChunk.fourCC";

    if(m_file.write(reinterpret_cast<char*>(&m_strhChunk.size), 4) == -1)
        qDebug() << "Error while writing m_strhChunk.size";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.fccType), 4) == -1)
        qDebug() << "Error while writing m_aviStream.fccType";

    if(m_file.write(reinterpret_cast<const char*>(m_aviStream.fccHandler), 4) == -1)
        qDebug() << "Error while writing m_aviStream.fccHandler";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.flags), 4) == -1)
        qDebug() << "Error while writing m_aviStream.flags";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.priority), 4) == -1)
        qDebug() << "Error while writing m_aviStream.priority";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.initialFrames), 4) == -1)
        qDebug() << "Error while writing m_aviStream.initialFrames";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.scale), 4) == -1)
        qDebug() << "Error while writing m_aviStream.scale";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.rate), 4) == -1)
        qDebug() << "Error while writing m_aviStream.rate";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.start), 4) == -1)
        qDebug() << "Error while writing m_aviStream.start";

    m_pos2 = m_file.pos();
    if(!m_pos2)
        qDebug() << "Error wrong position";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.length), 4) == -1)
        qDebug() << "Error while writing m_aviStream.length";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.suggestedBufferSize), 4) == -1)
        qDebug() << "Error while writing m_aviStream.suggestedBufferSize";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.quality), 4) == -1)
        qDebug() << "Error while writing m_aviStream.quality";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.sampleSize), 4) == -1)
        qDebug() << "Error while writing m_aviStream.sampleSize";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.language), 4) == -1)
        qDebug() << "Error while writing m_aviStream.language";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.language), 4) == -1)
        qDebug() << "Error while writing m_aviStream.language";

    if(m_file.write(reinterpret_cast<char*>(&m_strfChunk.fourCC), 4) == -1)
        qDebug() << "Error while writing m_strfChunk.fourCC";

    if(m_file.write(reinterpret_cast<char*>(&m_strfChunk.size), 4) == -1)
        qDebug() << "Error while writing m_strfChunk.size";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.size), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.size";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.width), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.width";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.height), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.height";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.planes), 2) == -1)
        qDebug() << "Error while writing m_streamFormat.planes";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.bitCount), 2) == -1)
        qDebug() << "Error while writing m_streamFormat.bitCount";

    if(m_file.write(reinterpret_cast<const char*>(m_streamFormat.compression), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.compression";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.sizeImage), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.sizeImage";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.xPelsPerMeter), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.xPelsPerMeter";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.yPelsPerMeter), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.yPelsPerMeter";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.clrUsed), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.clrUsed";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.clrImportant), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.clrImportant";

    /*Блок Junk*/
//    if(m_file.write(reinterpret_cast<char*>(&m_junk.fourCC), 4) == -1)
//        qDebug() << "Error while writing m_junk.fourCC";

//    if(m_file.write(reinterpret_cast<char*>(&m_junk.size), 4) == -1)
//        qDebug() << "Error while writing m_junk.size";

//    сами Junk данные
    /*Блок Junk*/

    //Запись аудио
    //Записи нет, т.к. Qt не умеет записывать с "аудиовыхода"
    //Запись аудио

    qint64 pos = m_file.pos();
    if(!pos)
        qDebug() << "Error wrong position";

    if(!m_file.seek(16))
        qDebug() << "Seek Error";

    m_hdrlList.size = static_cast<unsigned int>(pos) - 20; // 20 = 4(RIFF) + 4(RIFF Size) + 4(AVI) + 4(LIST) + 4(LIST Size);

    if(m_file.write(reinterpret_cast<char*>(&m_hdrlList.size), 4) == -1)
        qDebug() << "Error while writing main LIST size";

    if(!m_file.seek(pos))
        qDebug() << "Seek Error";

    //Блок JUNK(общий)

    if(m_file.write(reinterpret_cast<char*>(&m_moviList.list), 4) == -1)
        qDebug() << "Error while writing m_moviList.list";

    m_pos = m_file.pos();
    if(!m_pos)
        qDebug() << "Error wrong position";

    if(m_file.write(reinterpret_cast<char*>(&m_moviList.size), 4) == -1)
        qDebug() << "Error while writing m_moviList.size";

    if(m_file.write(reinterpret_cast<char*>(&m_moviList.fourCC), 4) == -1)
        qDebug() << "Error while writing m_moviList.fourCC";
    /*Записываем в файл*/


}

void AVIWriter::start(unsigned int width, unsigned int height, QString outputDirectory, int fps, const char* codec)
{
    m_isFpsSet = fps;
    qDebug() << m_isFpsSet;
    if(!(width && height))
        qDebug() << "frame size can't be null";

    /*Main Header*/
    if(!m_isFpsSet)
        m_mainHeader.microSecPerFrame = 0; //0 - т.к. мы пока не знаем реальный fps
    else
        m_mainHeader.microSecPerFrame = static_cast<uint>(1000000 / fps);
    m_mainHeader.maxBytesPerSec = width*height*4;
    m_mainHeader.paddingGranularity = 0;
    m_mainHeader.flags = 0x1001;
    m_mainHeader.totalFrames = 0; // Пока 0, т.к. еще не добавлено ни одного кадра
    m_mainHeader.initialFrames = 0;
    m_mainHeader.streams = 1; // Пока 1, потом в зависимости от кол-ва потоков
    m_mainHeader.suggestedBufferSize = width*height*4;
    m_mainHeader.width = width;
    m_mainHeader.height = height;
    /*Main Header*/

    /*Avi stream header*/
    m_aviStream.fccType = 0x73646976; // vids
    m_aviStream.fccHandler = static_cast<unsigned int>(reinterpret_cast<size_t>(codec)); //WARNING!!!
    m_aviStream.flags = 0;
    m_aviStream.priority = 0;
    m_aviStream.language = 0;
    m_aviStream.initialFrames = 0;
    m_aviStream.scale = 1;
    if(!m_isFpsSet)
        m_aviStream.rate = 0; // 0 - т.к. мы пока не знаем реальный fps
    else
        m_aviStream.rate = static_cast<uint>(fps);
    m_aviStream.start = 0;
    m_aviStream.length = 0; // пока 0, т.к. еще не добавлено ни одного кадра
    m_aviStream.suggestedBufferSize = width*height*4;
    m_aviStream.quality = -1; //WARNING: Possible problem
    m_aviStream.sampleSize = width*height*4;
    /*Avi stream header*/

    /*Avi stream format*/
    m_streamFormat.size = sizeof(AviStreamFormat);
    m_streamFormat.width = width;
    m_streamFormat.height = height;
    m_streamFormat.planes = 1;
    m_streamFormat.bitCount = 32; // Глянуть в каком формате и разрядности делает скриншоты Qt!!! 24???
    m_streamFormat.compression = m_aviStream.fccHandler;
    m_streamFormat.sizeImage = width*height*4; // width*height*3 ???
    m_streamFormat.xPelsPerMeter = 0;
    m_streamFormat.yPelsPerMeter = 0;
    m_streamFormat.clrUsed = 0;
    m_streamFormat.clrImportant = 0;
    /*Avi stream format*/

    /*Записываем в файл*/
    QString filename = QDateTime::currentDateTime().toString("ddMMyy-hhmmss") + ".avi";
    if(outputDirectory.isEmpty())
        m_file.setFileName(filename);
    else
        m_file.setFileName(outputDirectory+"/"+filename);
    if(!m_file.open(QIODevice::WriteOnly))
        qDebug() << "Error while opening file";

    if(m_file.write(reinterpret_cast<char*>(&m_aviList.list), 4) == -1)
        qDebug() << "Error while writing m_aviList.list";

    //Должно быть четыре 0, размер всего файла без последней секции junk
    if(m_file.write(reinterpret_cast<char*>(&m_aviList.size), 4) == -1)
        qDebug() << "Error while writing m_aviList.size";

    if(m_file.write(reinterpret_cast<char*>(&m_aviList.fourCC), 4) == -1)
        qDebug() << "Error while writing m_aviList.fourCC";

    if(m_file.write(reinterpret_cast<char*>(&m_hdrlList.list), 4) == -1)
        qDebug() << "Error while writing m_hdrlList.list";

    //Размер данных этого листа, т.е всех подзаголовков + секция junk, пока четыре 0
    if(m_file.write(reinterpret_cast<char*>(&m_hdrlList.size), 4) == -1)
        qDebug() << "Error while writing m_hdrlList.size";

    if(m_file.write(reinterpret_cast<char*>(&m_hdrlList.fourCC), 4) == -1)
        qDebug() << "Error while writing &m_hdrlList.fourCC";

    if(m_file.write(reinterpret_cast<char*>(&m_aviChunk.fourCC), 4) == -1)
        qDebug() << "Error while writing m_aviChunk.fourCC";

    if(m_file.write(reinterpret_cast<char*>(&m_aviChunk.size), 4) == -1)
        qDebug() << "Error while writing m_aviChunk.size";

    m_microSecPerFramePos = m_file.pos();
    if(!m_microSecPerFramePos)
        qDebug() << "m_microSecPerFramePos getting pos Error";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.microSecPerFrame), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.microSecPerFrame";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.maxBytesPerSec), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.maxBytesPerSec";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.paddingGranularity), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.paddingGranularity";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.flags), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.flags";

    // Пока 0, т.к. нету кадров
    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.totalFrames), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.totalFrames";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.initialFrames), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.initialFrames";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.streams), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.streams";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.suggestedBufferSize), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.suggestedBufferSize";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.width), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.width";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.height), 4) == -1)
        qDebug() << "Error while writing &m_mainHeader.height";

    for(int i = 0; i < 4; ++i)
        if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.reserved[i]), 4) == -1)
            qDebug() << "Error while writing m_mainHeader.reserved" << i;

    if(m_file.write(reinterpret_cast<char*>(&m_strlList.list), 4) == -1)
        qDebug() << "Error while writing m_strlList.list";

    if(m_file.write(reinterpret_cast<char*>(&m_strlList.size), 4) == -1)
        qDebug() << "Error while writing m_strlList.size";

    if(m_file.write(reinterpret_cast<char*>(&m_strlList.fourCC), 4) == -1)
        qDebug() << "Error while writing m_strlList.fourCC";

    if(m_file.write(reinterpret_cast<char*>(&m_strhChunk.fourCC), 4) == -1)
        qDebug() << "Error while writing m_strhChunk.fourCC";

    if(m_file.write(reinterpret_cast<char*>(&m_strhChunk.size), 4) == -1)
        qDebug() << "Error while writing m_strhChunk.size";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.fccType), 4) == -1)
        qDebug() << "Error while writing m_aviStream.fccType";

    if(m_file.write(reinterpret_cast<const char*>(m_aviStream.fccHandler), 4) == -1)
        qDebug() << "Error while writing m_aviStream.fccHandler";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.flags), 4) == -1)
        qDebug() << "Error while writing m_aviStream.flags";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.priority), 4) == -1)
        qDebug() << "Error while writing m_aviStream.priority";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.initialFrames), 4) == -1)
        qDebug() << "Error while writing m_aviStream.initialFrames";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.scale), 4) == -1)
        qDebug() << "Error while writing m_aviStream.scale";

    m_ratePos = m_file.pos();
    if(!m_ratePos)
        qDebug() << "m_ratePos getting pos error";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.rate), 4) == -1)
        qDebug() << "Error while writing m_aviStream.rate";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.start), 4) == -1)
        qDebug() << "Error while writing m_aviStream.start";

    m_pos2 = m_file.pos();
    if(!m_pos2)
        qDebug() << "Error wrong position";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.length), 4) == -1)
        qDebug() << "Error while writing m_aviStream.length";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.suggestedBufferSize), 4) == -1)
        qDebug() << "Error while writing m_aviStream.suggestedBufferSize";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.quality), 4) == -1)
        qDebug() << "Error while writing m_aviStream.quality";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.sampleSize), 4) == -1)
        qDebug() << "Error while writing m_aviStream.sampleSize";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.language), 4) == -1)
        qDebug() << "Error while writing m_aviStream.language";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.language), 4) == -1)
        qDebug() << "Error while writing m_aviStream.language";

    if(m_file.write(reinterpret_cast<char*>(&m_strfChunk.fourCC), 4) == -1)
        qDebug() << "Error while writing m_strfChunk.fourCC";

    if(m_file.write(reinterpret_cast<char*>(&m_strfChunk.size), 4) == -1)
        qDebug() << "Error while writing m_strfChunk.size";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.size), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.size";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.width), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.width";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.height), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.height";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.planes), 2) == -1)
        qDebug() << "Error while writing m_streamFormat.planes";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.bitCount), 2) == -1)
        qDebug() << "Error while writing m_streamFormat.bitCount";

    if(m_file.write(reinterpret_cast<const char*>(m_streamFormat.compression), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.compression";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.sizeImage), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.sizeImage";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.xPelsPerMeter), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.xPelsPerMeter";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.yPelsPerMeter), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.yPelsPerMeter";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.clrUsed), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.clrUsed";

    if(m_file.write(reinterpret_cast<char*>(&m_streamFormat.clrImportant), 4) == -1)
        qDebug() << "Error while writing m_streamFormat.clrImportant";

    /*Блок Junk*/
//    if(m_file.write(reinterpret_cast<char*>(&m_junk.fourCC), 4) == -1)
//        qDebug() << "Error while writing m_junk.fourCC";

//    if(m_file.write(reinterpret_cast<char*>(&m_junk.size), 4) == -1)
//        qDebug() << "Error while writing m_junk.size";

//    сами Junk данные
    /*Блок Junk*/

    //Запись аудио
    //Записи аудио нет, т.к. Qt не
    //Запись аудио

    qint64 pos = m_file.pos();
    if(!pos)
        qDebug() << "Error wrong position";

    if(!m_file.seek(16))
        qDebug() << "Seek Error";

    m_hdrlList.size = static_cast<unsigned int>(pos) - 20; // 20 = 4(RIFF) + 4(RIFF Size) + 4(AVI) + 4(LIST) + 4(LIST Size);

    if(m_file.write(reinterpret_cast<char*>(&m_hdrlList.size), 4) == -1)
        qDebug() << "Error while writing main LIST size";

    if(!m_file.seek(pos))
        qDebug() << "Seek Error";

    //Блок JUNK(общий)

    if(m_file.write(reinterpret_cast<char*>(&m_moviList.list), 4) == -1)
        qDebug() << "Error while writing m_moviList.list";

    m_pos = m_file.pos();
    if(!m_pos)
        qDebug() << "Error wrong position";

    if(m_file.write(reinterpret_cast<char*>(&m_moviList.size), 4) == -1)
        qDebug() << "Error while writing m_moviList.size";

    if(m_file.write(reinterpret_cast<char*>(&m_moviList.fourCC), 4) == -1)
        qDebug() << "Error while writing m_moviList.fourCC";
    /*Записываем в файл*/
}

void AVIWriter::addFrame(QImage *pix, int quality)
{
    unsigned int frameSize = 0;

    if(m_file.write("00dc") == -1)
        qDebug() << "Error while writing 00dc";

    qint64 posFrameSize = m_file.pos();

    if(m_file.write(reinterpret_cast<char*>(&frameSize), 4) == -1)
        qDebug() << "Error while writing frame size";

    pix->save(&m_file, "JPG", quality);

    qint64 posEndOfImg = m_file.pos();
    qint64 length = posEndOfImg - posFrameSize - 4;

    unsigned int paddingSize = 0;
    int padding = 0;

    if(length%4 != 0)
        paddingSize = 4 - (length%4);

    frameSize = static_cast<uint>(length)+paddingSize;

    m_framesSizes.append(frameSize);
    ++m_aviStream.length;


    if(!m_file.seek(posFrameSize))
        qDebug() << "Seek Error";

    if(m_file.write(reinterpret_cast<char*>(&frameSize), 4) == -1)
        qDebug() << "Error while writing frame size";

    if(!m_file.seek(posEndOfImg))
        qDebug() << "Seek Error";


    if(paddingSize)
        for(unsigned int i = 0; i < paddingSize; ++i)
            if(m_file.write(reinterpret_cast<char*>(&padding), 1) == -1)
                qDebug() << "Error while writing padding";
}

void AVIWriter::stop()
{
    qint64 pos = m_file.pos();
    if(!pos)
        qDebug() << "Error wrong position";

    if(!m_file.seek(m_pos))
        qDebug() << "Seek Error";

    m_moviList.size = static_cast<unsigned int>(pos) - static_cast<unsigned int>(m_pos) - 4;

    if(m_file.write(reinterpret_cast<char*>(&m_moviList.size), 4) == -1)
        qDebug() << "Error while writing m_moviList.size";

    if(!m_file.seek(pos))
        qDebug() << "Seek Error";

    if(m_file.write(reinterpret_cast<char*>(&m_indexChunk.fourCC), 4) == -1)
        qDebug() << "Error while writing m_indexChunk.fourCC";

    m_indexChunk.size = static_cast<uint>(m_framesSizes.size())*16;

    if(m_file.write(reinterpret_cast<char*>(&m_indexChunk.size), 4) == -1)
        qDebug() << "Error while writing m_indexChunk.size";

    int vSize = m_framesSizes.size();
    m_index.chunkOffset = 0x4;
    for(int i = 0; i < vSize; ++i) //Возможно лучше будет без использования структуры индекса
    {
        m_index.ckid = 0x63643030; //00dc
        if(m_file.write(reinterpret_cast<char*>(&m_index.ckid), 4) == -1) //пока только 00dc, пока нету аудио дороги
            qDebug() << "Error while writing m_index.ckid";

        m_index.flags = 0x10;
        if(m_file.write(reinterpret_cast<char*>(&m_index.flags), 4) == -1)
            qDebug() << "Error while writing m_index.flags";

        if(m_file.write(reinterpret_cast<char*>(&m_index.chunkOffset), 4) == -1)
            qDebug() << "Error while writing m_index.chunkOffset";

        m_index.chunkLength = m_framesSizes.at(i);
        if(m_file.write(reinterpret_cast<char*>(&m_index.chunkLength), 4) == -1)
            qDebug() << "Error while writing m_index.chunkLength";

        m_index.chunkOffset = m_index.chunkOffset + m_index.chunkLength + 8;
    }

    /*Junk*/
//    if(m_file.write(reinterpret_cast<char*>(&m_junk.fourCC), 4) == -1)
//        qDebug() << "Error while writing m_junk.fourCC";

//    if(m_file.write(reinterpret_cast<char*>(&m_junk.size), 4) == -1)
//        qDebug() << "Error while writing m_junk.size";
    /*Junk*/

    pos = m_file.pos();
    if(!pos)
        qDebug() << "Error wrong position";

    if(!m_file.seek(4))
        qDebug() << "Seek Error";

    m_aviList.size = static_cast<unsigned int>(pos) - 8; //если есть последняя секция Junk, то размер pos - m_junk.size - 16
    if(m_file.write(reinterpret_cast<char*>(&m_aviList.size), 4) == -1)
        qDebug() << "Error while writing m_aviList.size";

    m_mainHeader.totalFrames = m_aviStream.length;
    if(!m_file.seek(48))
        qDebug() << "Seek Error";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.totalFrames), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.totalFrames";

    if(!m_file.seek(m_pos2))
        qDebug() << "Seek Error";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.length), 4) == -1)
        qDebug() << "Error while writing m_aviStream.length";
    m_file.close();
}

void AVIWriter::stop(int time, int numberOfFrames)
{
    qint64 pos = m_file.pos();
    if(!pos)
        qDebug() << "Error wrong position";

    if(!m_file.seek(m_pos))
        qDebug() << "Seek Error";

    m_moviList.size = static_cast<unsigned int>(pos) - static_cast<unsigned int>(m_pos) - 4;

    if(m_file.write(reinterpret_cast<char*>(&m_moviList.size), 4) == -1)
        qDebug() << "Error while writing m_moviList.size";

    if(!m_file.seek(pos))
        qDebug() << "Seek Error";

    if(m_file.write(reinterpret_cast<char*>(&m_indexChunk.fourCC), 4) == -1)
        qDebug() << "Error while writing m_indexChunk.fourCC";

    m_indexChunk.size = static_cast<uint>(m_framesSizes.size())*16;

    if(m_file.write(reinterpret_cast<char*>(&m_indexChunk.size), 4) == -1)
        qDebug() << "Error while writing m_indexChunk.size";

    int vSize = m_framesSizes.size();
    m_index.chunkOffset = 0x4;
    for(int i = 0; i < vSize; ++i) //Возможно лучше будет без использования структуры индекса
    {
        m_index.ckid = 0x63643030; //00dc
        if(m_file.write(reinterpret_cast<char*>(&m_index.ckid), 4) == -1) //пока только 00dc, пока нету аудио дороги
            qDebug() << "Error while writing m_index.ckid";

        m_index.flags = 0x10;
        if(m_file.write(reinterpret_cast<char*>(&m_index.flags), 4) == -1)
            qDebug() << "Error while writing m_index.flags";

        if(m_file.write(reinterpret_cast<char*>(&m_index.chunkOffset), 4) == -1)
            qDebug() << "Error while writing m_index.chunkOffset";

        m_index.chunkLength = m_framesSizes.at(i);
        if(m_file.write(reinterpret_cast<char*>(&m_index.chunkLength), 4) == -1)
            qDebug() << "Error while writing m_index.chunkLength";

        m_index.chunkOffset = m_index.chunkOffset + m_index.chunkLength + 8;
    }

    /*Junk*/
//    if(m_file.write(reinterpret_cast<char*>(&m_junk.fourCC), 4) == -1)
//        qDebug() << "Error while writing m_junk.fourCC";

//    if(m_file.write(reinterpret_cast<char*>(&m_junk.size), 4) == -1)
//        qDebug() << "Error while writing m_junk.size";
    /*Junk*/

    pos = m_file.pos();
    if(!pos)
        qDebug() << "Error wrong position";

    if(!m_file.seek(4))
        qDebug() << "Seek Error";

    m_aviList.size = static_cast<unsigned int>(pos) - 8; //если есть последняя секция Junk, то размер pos - m_junk.size - 16
    if(m_file.write(reinterpret_cast<char*>(&m_aviList.size), 4) == -1)
        qDebug() << "Error while writing m_aviList.size";

    m_mainHeader.totalFrames = m_aviStream.length;
    if(!m_file.seek(48))
        qDebug() << "Seek Error";

    if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.totalFrames), 4) == -1)
        qDebug() << "Error while writing m_mainHeader.totalFrames";

    if(!m_file.seek(m_pos2))
        qDebug() << "Seek Error";

    if(m_file.write(reinterpret_cast<char*>(&m_aviStream.length), 4) == -1)
        qDebug() << "Error while writing m_aviStream.length";

    if(!m_file.seek(m_ratePos))
        qDebug() << "m_ratePos Seek Error";

    if(!m_isFpsSet)
    {
        m_aviStream.rate = static_cast<unsigned int>(numberOfFrames/time);
        if(m_file.write(reinterpret_cast<char*>(&m_aviStream.rate), 4) == -1)
            qDebug() << "Error while writing m_aviStream.rate";

        if(!m_file.seek(m_microSecPerFramePos))
            qDebug() << "m_microSecPerFramePos Seek Error";

        m_mainHeader.microSecPerFrame = 1000000/m_aviStream.rate;
        if(m_file.write(reinterpret_cast<char*>(&m_mainHeader.microSecPerFrame), 4) == -1)
            qDebug() << "Error while writing m_mainHeader.microSecPerFrame";
    }

    m_file.close();
}
