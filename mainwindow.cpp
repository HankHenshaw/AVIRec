#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_outputDir("")
{
    ui->setupUi(this);

    m_screens = QGuiApplication::screens();
    m_isDone = false;
    m_frames = 0;
    m_fps = 0;
    m_lockedFps = 0;

    ui->spinBoxThreads->setRange(1, QThread::idealThreadCount());
    ui->spinBoxMonitors->setRange(1, m_screens.size());
    ui->spinBoxQuality->setRange(0, 100);
    ui->spinBoxQuality->setDisabled(true);
    ui->lineEditFolder->setPlaceholderText("If the string is empty, the default folder will be used.");
    ui->lineEditFolder->setReadOnly(true);
    ui->spinBoxFPS->setRange(1, 100);
    ui->spinBoxFPS->setValue(25);
    ui->spinBoxFPS->setDisabled(true);
    ui->spinBoxLockFPS->setRange(1, 100);
    ui->spinBoxLockFPS->setValue(25);
    ui->spinBoxLockFPS->setDisabled(true);

    this->setWindowTitle("AVIRec");

    m_codecList << "1978" << "2VUY" << "3IV0" << "3IV1" << "3IV2" << "3IVD" << "3IVX" << "8BPS" << "AAS4" << "AASC" << "ABYR" << "ACTL" << "ADV1"
                << "ADVJ" << "AEIK" << "AEMI" << "AFLC" << "AFLI" << "AHDV" << "AJPG" << "AMPG" << "ANIM" << "AP41" << "AP42" << "ASLC" << "ASV1"
                << "ASV2" << "ASVX" << "ATM4" << "AUR2" << "AURA" << "AVC1" << "AVRN" << "BA81" << "BINK" << "BLZ0" << "BT20" << "BTCV" << "BW10"
                << "BYR1" << "BYR2" << "CC12" << "CDVC" << "CFCC" << "CGDI" << "CHAM" << "CJPG" << "CMYK" << "CPLA" << "CRAM" << "CSCD" << "CTRX"
                << "CVID" << "CWLT" << "CXY1" << "CXY2" << "CYUY" << "D261" << "D263" << "DAVC" << "DCL1" << "DCL2" << "DCL3" << "DCL4" << "DCL4"
                << "DIV3" << "DIV4" << "DIV5" << "DIVX" << "DM4V" << "DMB1" << "DMB2" << "DMK2" << "DSVD" << "DUCK" << "DV25" << "DV50" << "DVAN"
                << "DVCS" << "DVE2" << "DVH1" << "DVHD" << "DVSD" << "DVSL" << "DVX1" << "DVX2" << "DVX3" << "DX50" << "DXGM" << "DXTC" << "DXTN"
                << "EKQ0" << "ELK0" << "EM2V" << "ES07" << "ESCP" << "ETV1" << "ETV2" << "ETVC" << "FFV1" << "FLJP" << "FMP4" << "FMVC" << "FPS1"
                << "FRWA" << "FRWD" << "FVF1" << "GEOX" << "GJPG" << "GLZW" << "GPEG" << "GWLT" << "H260" << "H261" << "H262" << "H263" << "H264"
                << "H265" << "H266" << "H267" << "H268" << "H269" << "HDYC" << "HEVC" << "HFYU" << "HMCR" << "HMRR" << "I263" << "ICLB" << "IGOR"
                << "IJLV" << "IJPG" << "ILVC" << "ILVR" << "IPDV" << "IPMA" << "IR21" << "IRAW" << "ISME" << "IV30" << "IV31" << "IV32" << "IV33"
                << "IV34" << "IV35" << "IV36" << "IV37" << "IV38" << "IV39" << "IV40" << "IV41" << "IV42" << "IV43" << "IV44" << "IV45" << "IV46"
                << "IV47" << "IV48" << "IV49" << "IV50" << "JBYR" << "JPEG" << "JPGL" << "KB2A" << "KB2D" << "KB2F" << "KB2G" << "KMVC" << "L261"
                << "L263" << "LBYR" << "LCMW" << "LCW2" << "LEAD" << "LGRY" << "LJ11" << "LJ22" << "LJ2K" << "LJ44" << "LJPG" << "LMP2" << "LMP4"
                << "LSVC" << "LSVX" << "LZO1" << "M261" << "M263" << "M4CC" << "M4S2" << "MC12" << "MCAM" << "MJ2C" << "MJPG" << "MMES" << "MP2A"
                << "MP2T" << "MP2V" << "MP42" << "MP43" << "MP4A" << "MP4S" << "MP4T" << "MP4V" << "MPEG" << "MPG4" << "MPGI" << "MR16" << "MRCA"
                << "MRLE" << "MSVC" << "MSZH" << "MTX1" << "MTX2" << "MTX3" << "MTX4" << "MTX5" << "MTX6" << "MTX7" << "MTX8" << "MTX9" << "MVI1"
                << "MVI2" << "MWV1" << "NAVI" << "NDSC" << "NDSM" << "NDSP" << "NDSS" << "NDXC" << "NDXH" << "NDXP" << "NDXS" << "NHVU" << "NI24"
                << "NTN1" << "NTN2" << "NVDS" << "NVH2" << "NVS0" << "NVS1" << "NVS2" << "NVS3" << "NVS4" << "NVS5" << "NVT0" << "NVT1" << "NVT2"
                << "NVT3" << "NVT4" << "NVT5" << "PDVC" << "PGVV" << "PHMO" << "PIM1" << "PIM2" << "PIMJ" << "PIXL" << "PJPG" << "PVEZ" << "PVMM"
                << "PVW2" << "QPEG" << "QPEQ" << "RGBT" << "RLE " << "RLE4" << "RLE8" << "RMP4" << "RPZA" << "RT21" << "RV20" << "RV30" << "RV40"
                << "S422" << "SAN3" << "SDCC" << "SEDG" << "SFMC" << "SMK2" << "SMK4" << "SMKA" << "SMP4" << "SMSC" << "SMSD" << "SMSV" << "SP40"
                << "SP44" << "SP54" << "SPIG" << "SQZ2" << "STVA" << "STVB" << "STVC" << "STVX" << "STVY" << "SV10" << "SVQ1" << "SVQ3" << "TLMS"
                << "TLST" << "TM20" << "TM2X" << "TMIC" << "TMOT" << "TR20" << "TSCC" << "TV10" << "TVJP" << "TVMJ" << "TY0N" << "TY2C" << "TY2N"
                << "UCOD" << "ULTI" << "V210" << "V261" << "V655" << "VCR1" << "VCR2" << "VCR3" << "VCR4" << "VCR5" << "VCR6" << "VCR7" << "VCR8"
                << "VCR9" << "VDCT" << "VDOM" << "VDOW" << "VDTZ" << "VGPX" << "VIDS" << "VIFP" << "VIVO" << "VIXL" << "VLV1" << "VP30" << "VP31"
                << "VP40" << "VP50" << "VP60" << "VP61" << "VP62" << "VP70" << "VP80" << "VQC1" << "VQC2" << "VQJC" << "VSSV" << "VUUU" << "VX1K"
                << "VX2K" << "VXSP" << "VYU9" << "VYUY" << "WBVC" << "WHAW" << "WINX" << "WJPG" << "WMV1" << "WMV2" << "WMV3" << "WMVA" << "WNV1"
                << "WVC1" << "X263" << "X264" << "XLV0" << "XMPG" << "XVID" << "XWV0" << "XWV1" << "XWV2" << "XWV3" << "XWV4" << "XWV5" << "XWV5"
                << "XWV6" << "XWV7" << "XWV8" << "XWV9" << "XXAN" << "Y16 " << "Y411" << "Y41P" << "Y444" << "Y8  " << "YC12" << "YUV8" << "YUV9"
                << "YUVP" << "YUY2" << "YUYV" << "YV12" << "YV16" << "YV92" << "ZLIB" << "ZMBV" << "ZPEG" << "ZYGO" << "ZYYY";

    foreach(QString codec, m_codecList)
        ui->comboCodecBox->addItem(codec);

    ui->comboCodecBox->setCurrentIndex(192);

    for(int i = 0; i < QThread::idealThreadCount(); ++i)
    {
        m_vecImage.push_back(QImage());
        m_futureVecImage.push_back(QFuture<QImage>());
    }

    //TODO: При повторной записи возникает артефакты
    //TODO: FPS Lock выдает не совсем точный результат, но близко к выбранному
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonStart_clicked()
{
    qDebug() << "Start";
    m_isDone = false;
    m_frames = 0;
    if(ui->checkBoxSetFPS->checkState())
        m_fps = ui->spinBoxFPS->value();
    if(ui->checkBoxLockFPS->checkState())
        m_lockedFps = ui->spinBoxLockFPS->value();

    int numberOfThreads = ui->spinBoxThreads->value();
    int quality = -1;
    if(ui->checkBoxQuality->checkState())
        quality = ui->spinBoxQuality->value();

    qDebug() << "Threads:" << numberOfThreads;

    for(int i = 0; i < numberOfThreads; ++i)
        m_futureVecImage[i] = QtConcurrent::run(this, &MainWindow::takeFrame);

    m_writer.start(static_cast<uint>(m_screens.at(ui->spinBoxMonitors->value()-1)->size().width()), static_cast<uint>(m_screens.at(ui->spinBoxMonitors->value()-1)->size().height()), m_outputDir, m_fps, ui->comboCodecBox->currentText().toUtf8().data());
    if(m_lockedFps)
    {
        qDebug() << "Locked";
        if(numberOfThreads > 1)
        {
            int period = 1000/m_lockedFps;
            int realPeriod = 0;
            int totalTime = 0;
            int iteration = 0;
            m_timer.start();
            while(!m_isDone)
            {
                for(int i = 0; i < numberOfThreads; ++i)
                {
                    ++iteration;
                    realPeriod = m_timer.elapsed();
                    m_timer.start();
                    totalTime += realPeriod;
                    if(totalTime < period*iteration)
                    {
                        ulong sleep = period*iteration-totalTime;
                        QThread::msleep(sleep);
                    }

                    m_vecImage[i] = m_futureVecImage.at(i).result();
                    m_writer.addFrame(&m_vecImage[i], quality);

                    m_futureVecImage[((numberOfThreads+i-1)%numberOfThreads)] = QtConcurrent::run(this, &MainWindow::takeFrame);
                    ++m_frames;

                    qApp->processEvents();
                }
            }
            qDebug() << "Time:" << m_timer.elapsed();
            qDebug() << "Frames:" << m_frames;
            qDebug() << "Total Time: " << totalTime;
            float time = totalTime/1000;
            qDebug() << "FPS:" << m_frames/time;
            m_writer.stop(time, m_frames);
        }
        else if(numberOfThreads == 1)
        {
            int period = 1000/m_lockedFps;
            int realPeriod = 0;
            int totalTime = 0;
            int iteration = 0;
            m_timer.start();
            while (!m_isDone)
            {
                ++iteration;
                realPeriod = m_timer.elapsed();
                m_timer.start();
                totalTime += realPeriod;
                if(totalTime < period*iteration)
                {
                    ulong sleep = period*iteration-totalTime;
                    QThread::msleep(sleep);
                }
                m_frame = m_screens.at(ui->spinBoxMonitors->value()-1)->grabWindow(0).toImage();
                m_writer.addFrame(&m_frame, quality);

                ++m_frames;

                qApp->processEvents();
            }
            qDebug() << "Time:" << m_timer.elapsed();
            qDebug() << "Frames:" << m_frames;
            qDebug() << "Total Time: " << totalTime;
            float time = totalTime/1000;
            qDebug() << "FPS:" << m_frames/time;
            m_writer.stop(time, m_frames);
        }
        else
        {
            qDebug() << "Something went wrong. Number of threads is less than 1";
        }
    }
    else
    {
        qDebug() << "Not Locked";
        if(numberOfThreads > 1)
        {
            m_timer.start();
            while(!m_isDone)
            {
                for(int i = 0; i < numberOfThreads; ++i)
                {
                    m_vecImage[i] = m_futureVecImage.at(i).result();
                    m_writer.addFrame(&m_vecImage[i], quality);

                    m_futureVecImage[((numberOfThreads+i-1)%numberOfThreads)] = QtConcurrent::run(this, &MainWindow::takeFrame);
                    ++m_frames;

                    qApp->processEvents();
                }
            }
            qDebug() << "Time:" << m_timer.elapsed();
            qDebug() << "Frames:" << m_frames;

            float time = m_timer.elapsed()/1000;
            qDebug() << "FPS:" << m_frames/time;
            m_writer.stop(time, m_frames);
        }
        else if(numberOfThreads == 1)
        {
            m_timer.start();
            while(!m_isDone)
            {
                m_frame = m_screens.at(ui->spinBoxMonitors->value()-1)->grabWindow(0).toImage();
                m_writer.addFrame(&m_frame, quality);

                ++m_frames;

                qApp->processEvents();
            }
            qDebug() << "Time:" << m_timer.elapsed();
            qDebug() << "Frames:" << m_frames;

            float time = m_timer.elapsed()/1000;
            qDebug() << "FPS:" << m_frames/time;
            m_writer.stop(time, m_frames);
        }
        else
        {
            qDebug() << "Something went wrong. Number of threads is less than 1";
        }
    }
    m_frames = 0;
}

void MainWindow::on_buttonStop_clicked()
{
    qDebug() << "Stop";
    m_isDone = true;
    //Подумать об проблемемах при аварийном завершении программы
}

QImage MainWindow::takeFrame()
{
    qDebug() << "ThreadID" << QThread::currentThreadId();
    m_frame = m_screens.at(ui->spinBoxMonitors->value()-1)->grabWindow(0).toImage();
    return m_frame;
}

void MainWindow::on_checkBoxQuality_stateChanged(int arg1)
{
    if(!arg1)
        ui->spinBoxQuality->setDisabled(true);
    else if (arg1 == 2)
        ui->spinBoxQuality->setEnabled(true);
}

void MainWindow::on_pushButtonFile_clicked()
{
    qDebug() << "File Dialog Test";
    QString outputDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    ui->lineEditFolder->setText(outputDir);
    qDebug() << outputDir;
}

void MainWindow::on_lineEditFolder_textChanged(const QString &arg1)
{
    qDebug() << "Text Changed";
    m_outputDir = arg1;
}

void MainWindow::on_checkBoxSetFPS_stateChanged(int arg1)
{
    if(!arg1)
        ui->spinBoxFPS->setDisabled(true);
    else if(arg1 == 2)
        ui->spinBoxFPS->setEnabled(true);
}

void MainWindow::on_checkBoxLockFPS_stateChanged(int arg1)
{
    if(!arg1)
        ui->spinBoxLockFPS->setDisabled(true);
    else if(arg1 == 2)
        ui->spinBoxLockFPS->setEnabled(true);
}
