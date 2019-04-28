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
#include <algorithm>
#include <cmath>
#include <qmath.h>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QImage>
#include <QRgb>
#include <QColor>

/*extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}*/

const QString COL_SEPARATOR = ";";
const QString ROW_SEPARATOR = "\n";

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

bool MainWindow::isAnyMethodSelected() {
    if (ui->selectedMethodMathExpecting->isChecked()) {
        return true;
    }
    if (ui->selectedMethodSup->isChecked()) {
        return true;
    }
    if (ui->selectedMethodPSNR->isChecked()) {
        return true;
    }
    if (ui->selectedMethodSSIM->isChecked()) {
        return true;
    }
    return false;
}

QString MainWindow::getVideoFrameCommand(QString fileIn, QString fileOut, QString timestamp) {
    fileIn.replace(" ", "\\ ");
    fileOut.replace(" ", "\\ ");

    return "ffmpeg -ss " + timestamp + " -i " +
            fileIn + " -vframes 1 -q:v 2 /home/paswd/logs/" +
            fileOut + " -y -loglevel quiet";
}

qreal MainWindow::getIntensity(QRgb pixel) {
    return (.3 * (qreal) qRed(pixel)) + (.59 * (qreal) qGreen(pixel)) +
            (.11 * (qreal) qBlue(pixel));
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
    //ui->videoLength->setText(getFrameCompressedCmd);
    system(qPrintable(getFrameBasicCmd));
    system(qPrintable(getFrameCompressedCmd));
}
void MainWindow::removeTempFrameFiles() {
    system(qPrintable("rm " + TMP_IMAGE_BASIC_FILENAME));
    system(qPrintable("rm " + TMP_IMAGE_COMPRESSED_FILENAME));
}

qreal MainWindow::pixelDiff(QRgb a, QRgb b) {
    return getIntensity(a) - getIntensity(b);
}

int MainWindow::getProgressValue(size_t value, size_t maxValue) {
    qreal percent = (qreal) maxValue / 100.;
    return (int) (value / percent);
}

qreal MainWindow::frameAssessmentMathExpecting(QImage &basic, QImage &compressed) {
    qreal summDiff = 0.;
    size_t height = qMin(basic.height(), compressed.height());
    size_t width = qMin(basic.width(), compressed.width());

    for (size_t h = 0; h < height; h++) {
        for (size_t w = 0; w < width; w++) {
            QRgb basicPixel = basic.pixel(w, h);
            QRgb compressedPixel = compressed.pixel(w, h);

            //summDiff += getIntensity(basicPixel) - getIntensity(compressedPixel);
            summDiff += pixelDiff(basicPixel, compressedPixel);

            //ui->progressBar->setValue(getProgressValue(h * width + w, height * width));
        }
    }

    return summDiff / (height * width);
}

qreal MainWindow::frameAssessmentSup(QImage &basic, QImage &compressed) {
    qreal maxDiff = 0.;
    size_t height = qMin(basic.height(), compressed.height());
    size_t width = qMin(basic.width(), compressed.width());

    for (size_t h = 0; h < height; h++) {
        for (size_t w = 0; w < width; w++) {
            QRgb basicPixel = basic.pixel(w, h);
            QRgb compressedPixel = compressed.pixel(w, h);

            //summDiff += getIntensity(basicPixel) - getIntensity(compressedPixel);
            qreal currDiff = pixelDiff(basicPixel, compressedPixel);
            maxDiff = (currDiff > maxDiff ? currDiff : maxDiff);

            //ui->progressBar->setValue(getProgressValue(h * width + w, height * width));
        }
    }
    return maxDiff;
}

qreal MainWindow::frameAssessmentPSNR(QImage &basic, QImage &compressed) {
    qreal summSqrDiff = 0;
    size_t height = qMin(basic.height(), compressed.height());
    size_t width = qMin(basic.width(), compressed.width());

    for (size_t h = 0; h < height; h++) {
        for (size_t w = 0; w < width; w++) {
            QRgb basicPixel = basic.pixel(w, h);
            QRgb compressedPixel = compressed.pixel(w, h);

            //summDiff += getIntensity(basicPixel) - getIntensity(compressedPixel);
            qreal currDiff = pixelDiff(basicPixel, compressedPixel);
            summSqrDiff += currDiff * currDiff;

            //ui->progressBar->setValue(getProgressValue(h * width + w, height * width));
        }
    }

    qreal frac = (255 * height * width) / qSqrt(summSqrDiff);
    return 20. * (qreal) log10((double) frac);
}

qreal MainWindow::frameAssessmentSSIM(QImage &basic, QImage &compressed) {
    return 0.;
}

QString MainWindow::getFrameAssessmentValue(size_t currTime) {
    generateTempFrameFiles(currTime);
    //QPixmap basicPicture(TMP_IMAGE_BASIC_FILENAME);
    //QPixmap compressedPicture(TMP_IMAGE_COMPRESSED_FILENAME);
    QImage basicPicture(TMP_IMAGE_BASIC_FILENAME);
    QImage compressedPicture(TMP_IMAGE_COMPRESSED_FILENAME);

    if (ui->selectedMethodMathExpecting->isChecked()) {
        //return frameAssessmentMathExpecting();
        return QString::number(currTime) + "," +
                QString::number(frameAssessmentMathExpecting(basicPicture, compressedPicture))
                + "\n";
    }

    if (ui->selectedMethodSup->isChecked()) {
        //return frameAssessmentSup();
        return QString::number(currTime) + COL_SEPARATOR +
                QString::number(frameAssessmentSup(basicPicture, compressedPicture))
                + ROW_SEPARATOR;
    }

    if (ui->selectedMethodPSNR->isChecked()) {
        //return frameAssessmentPSNR();
        return QString::number(currTime) + COL_SEPARATOR +
                QString::number(frameAssessmentPSNR(basicPicture, compressedPicture))
                + ROW_SEPARATOR;
    }

    if (ui->selectedMethodSSIM->isChecked()) {
        //return frameAssessmentSSIM();
        return QString::number(currTime) + COL_SEPARATOR +
                QString::number(frameAssessmentSSIM(basicPicture, compressedPicture))
                + ROW_SEPARATOR;
    }
    return "";
}

QString MainWindow::getCSVOutputData() {
    size_t videoLength = ui->videoLength->text().toInt();
    QString res = "Second" + COL_SEPARATOR + "Value" + ROW_SEPARATOR;
    for (size_t currTime = 0; currTime < videoLength; currTime++) {
        res += getFrameAssessmentValue(currTime);
        ui->progressBar->setValue(getProgressValue(currTime, videoLength));
    }
    removeTempFrameFiles();
    return res;
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
    if (!isAnyMethodSelected()) {
        QMessageBox::about(this, "Status", "No method selected");
        return;
    }
    ui->calculate->setDisabled(true);
    ui->basicVideoFileSelectBtn->setDisabled(true);
    ui->compressedVideoFileSelectBtn_2->setDisabled(true);
    ui->resultSaveSelectBtn->setDisabled(true);
    //ui->progressBar->setEnabled(true);
    ui->progressBar->setValue(0);
    setCursor(Qt::WaitCursor);
    QFile fout(ui->resultSavePath->text());

    if (fout.open(QIODevice::ReadWrite)) {
        QTextStream stream(&fout);
        stream << getCSVOutputData();
        setCursor(Qt::ArrowCursor);
        ui->progressBar->setValue(100);
        QMessageBox::about(this, "Status", "Successfully completed");
    } else {
        setCursor(Qt::ArrowCursor);
        QMessageBox::about(this, "Status", "Saving error");
    }
    ui->progressBar->setValue(0);
    //ui->progressBar->setEnabled(false);
    ui->calculate->setDisabled(false);
    ui->basicVideoFileSelectBtn->setDisabled(false);
    ui->compressedVideoFileSelectBtn_2->setDisabled(false);
    ui->resultSaveSelectBtn->setDisabled(false);
}

void MainWindow::on_resultSaveSelectBtn_clicked()
{
    //QString filePath = QFileDialog::getOpenFileName(0, "Save", "", "*.bmp");
    QString filePath = QFileDialog::getSaveFileName(0, "Save", "result.csv", "*.csv");
    //ui->compressedVideoFilePath->setText(filePath);
    ui->resultSavePath->setText(filePath);
}
