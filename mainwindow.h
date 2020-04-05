#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QtConcurrent>
#include <QImage>
#include <QTime>
#include <QScreen>
#include <QFuture>
#include <QFileDialog>
#include "aviwriter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_buttonStart_clicked();
    void on_buttonStop_clicked();
    void on_checkBoxQuality_stateChanged(int arg1);
    void on_pushButtonFile_clicked();
    void on_lineEditFolder_textChanged(const QString &arg1);
    void on_checkBoxSetFPS_stateChanged(int arg1);
    void on_checkBoxLockFPS_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QTime m_timer;
    QVector<QImage> m_vecImage;
    QVector<QFuture<QImage>> m_futureVecImage;
    QList<QScreen*> m_screens;
    bool m_isDone;
    int m_frames;
    QImage m_frame;
    QString m_outputDir;
    int m_fps;
    int m_lockedFps;
    QStringList m_codecList;

    QImage takeFrame();
};

#endif // MAINWINDOW_H
