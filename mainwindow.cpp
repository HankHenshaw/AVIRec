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

    for(int i = 0; i < QThread::idealThreadCount(); ++i)
    {
        m_vecImage.push_back(QImage());
        m_futureVecImage.push_back(QFuture<QImage>());
    }

    //TODO: При повторной записи возникает артефакты (возможно я это победил, но теперь получается криво высчитывается время проигрывания)
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

    m_writer.start(static_cast<uint>(m_screens.at(ui->spinBoxMonitors->value()-1)->size().width()), static_cast<uint>(m_screens.at(ui->spinBoxMonitors->value()-1)->size().height()), m_outputDir, m_fps);
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
                    m_writer.addFrame2(&m_vecImage[i], quality);

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
                m_writer.addFrame2(&m_frame, quality);

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
                    m_writer.addFrame2(&m_vecImage[i], quality);

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
                m_writer.addFrame2(&m_frame, quality);

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
