/*
 ==================================================
| __________                 __      __________    |
| \______   \_____    ______/  \    /  \______ \   |
|  |     ___/\__  \  /  ___/\   \/\/   /|    |  \  |
|  |    |     / __ \_\___ \  \        / |    `   \ |
|  |____|    (____  /____  >  \__/\  / /_______  / |
|                \/     \/        \/          \/   |
 ==================================================
(c) Developed by Pavel Sushko (PasWD)
http://paswd.ru/
me@paswd.ru

Original repository:
https://github.com/paswd/VideoAssessment

All rights reserved
*/

//Before starting install packages:
// * libavcodec-dev
// * libavformat-dev
// * libswscale-dev

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

/*extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}*/

const QString TMP_IMAGE_BASIC_FILENAME = "basicFrame.bmp";
const QString TMP_IMAGE_COMPRESSED_FILENAME = "compressedFrame.bmp";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //av_register_all();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getVideoFrameCommand(QString fileIn, QString fileOut, QString timestamp) {
    fileIn.replace(" ", "\\ ");
    fileOut.replace(" ", "\\ ");

    return "ffmpeg -ss " + timestamp + " -i " +
            fileIn + " -vframes 1 -q:v 2 /home/paswd/logs/" +
            fileOut + " -y -loglevel quiet";
}

void MainWindow::generateTempFrameFiles(size_t currTime) {
    size_t currHour = currTime / 3600;
    size_t currMinute = (currTime / 60) % 60;
    size_t currSecond = currTime % 60;
    QString currTimeStr = QString::number(currHour) + ":" +
            QString::number(currMinute) + ":" +
            QString::number(currSecond);

    QString getFrameBasicCmd = getVideoFrameCommand(ui->basicVideoFilePath->text(),
            TMP_IMAGE_BASIC_FILENAME, currTimeStr);
    QString getFrameCompressedCmd = getVideoFrameCommand(ui->compressedVideoFilePath->text(),
            TMP_IMAGE_COMPRESSED_FILENAME, currTimeStr);
    ui->videoLength->setText(getFrameCompressedCmd);
    system(qPrintable(getFrameBasicCmd));
    system(qPrintable(getFrameCompressedCmd));
}

QString MainWindow::getCSVOutputData() {
    size_t videoLength = ui->videoLength->text().toInt();
    for (size_t currTime = 0; currTime < videoLength; currTime++) {
        generateTempFrameFiles(currTime);
    }
    return "ABC";
}

void MainWindow::on_basicVideoFileSelectBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(0, "Open", "", "*.avi");
    ui->basicVideoFilePath->setText(filePath);
}

void MainWindow::on_compressedVideoFileSelectBtn_2_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(0, "Open", "", "*.avi");
    ui->compressedVideoFilePath->setText(filePath);
}

void MainWindow::on_calculate_clicked()
{
    QFile fout(ui->resultSavePath->text());
    if (fout.open(QIODevice::ReadWrite)) {
        QTextStream stream(&fout);
        stream << getCSVOutputData();
        QMessageBox::about(this, "Status", "Successfully completed");
    } else {
        QMessageBox::about(this, "Status", "Saving error");
    }
}

void MainWindow::on_resultSaveSelectBtn_clicked()
{
    //QString filePath = QFileDialog::getOpenFileName(0, "Save", "", "*.bmp");
    QString filePath = QFileDialog::getSaveFileName(0, "Save", "result.csv", "*.csv");
    //ui->compressedVideoFilePath->setText(filePath);
    ui->resultSavePath->setText(filePath);
}
