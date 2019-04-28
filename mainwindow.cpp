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

const QString COL_SEPARATOR                 = ";";
const QString ROW_SEPARATOR                 = "\n";
const QString DOUBLE_NUM_SEPARATOR          = ",";

const QString SUPPORTED_FILE_TYPES          = "*.avi *.mp4 *.mkv *.flv";

const QString TMP_IMAGE_BASIC_FILENAME      = "basicFrame.bmp";
const QString TMP_IMAGE_COMPRESSED_FILENAME = "compressedFrame.bmp";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setEditableElementsDisabled(bool disabled) {
    //Buttons
    ui->calculate->setDisabled(disabled);
    ui->basicVideoFileSelectBtn->setDisabled(disabled);
    ui->compressedVideoFileSelectBtn_2->setDisabled(disabled);
    ui->resultSaveSelectBtn->setDisabled(disabled);

    //Text areas
    ui->basicVideoFilePath->setDisabled(disabled);
    ui->compressedVideoFilePath->setDisabled(disabled);
    ui->resultSavePath->setDisabled(disabled);
    ui->videoLength->setDisabled(disabled);

    //Radio buttons
    ui->selectedMethodMathExpecting->setDisabled(disabled);
    ui->selectedMethodSup->setDisabled(disabled);
    ui->selectedMethodPSNR->setDisabled(disabled);
    ui->selectedMethodSSIM->setDisabled(disabled);
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
            fileIn + " -vframes 1 -q:v 2 " +
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

            summDiff += pixelDiff(basicPixel, compressedPixel);
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

            qreal currDiff = pixelDiff(basicPixel, compressedPixel);
            maxDiff = (currDiff > maxDiff ? currDiff : maxDiff);
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

            qreal currDiff = pixelDiff(basicPixel, compressedPixel);
            summSqrDiff += currDiff * currDiff;
        }
    }

    qreal frac = (255 * height * width) / qSqrt(summSqrDiff);
    return 20. * (qreal) log10((double) frac);
}

qreal MainWindow::frameAssessmentSSIM(QImage &basic, QImage &compressed) {
    //return 0.;
    size_t height = qMin(basic.height(), compressed.height());
    size_t width = qMin(basic.width(), compressed.width());
    size_t size = height * width;

    //Mid values
    qreal basicMid = 0.;
    qreal compressedMid = 0.;
    for (size_t h = 0; h < height; h++) {
        for (size_t w = 0; w < width; w++) {
            QRgb basicPixel = basic.pixel(w, h);
            QRgb compressedPixel = compressed.pixel(w, h);

            basicMid += (qreal) getIntensity(basicPixel);
            compressedMid += (qreal) getIntensity(compressedPixel);
        }
    }
    basicMid /= (qreal) size;
    compressedMid /= (qreal) size;

    //Dispersion & covariance
    qreal basicDisp = 0.;
    qreal compressedDisp = 0.;
    qreal cov = 0.;

    for (size_t h = 0; h < height; h++) {
        for (size_t w = 0; w < width; w++) {
            QRgb basicPixel = basic.pixel(w, h);
            QRgb compressedPixel = compressed.pixel(w, h);

            basicDisp += qPow((qreal) getIntensity(basicPixel) - basicMid, 2.);
            compressedDisp += qPow((qreal) getIntensity(compressedPixel) - compressedMid, 2.);
            cov += ((qreal) getIntensity(basicPixel) - basicMid) *
                    ((qreal) getIntensity(compressedPixel) - compressedMid);
        }
    }

    basicDisp /= ((qreal) size - 1.);
    compressedDisp /= ((qreal) size - 1.);
    cov /= ((qreal) size - 1.);

    const qreal c1 = 255. * .01;
    const qreal c2 = 255. * .03;

    qreal basicMidSq = basicMid * basicMid;
    qreal compressedMidSq = compressedMid * compressedMid;

    return ((2 * basicMid * compressedMid + c1) * (2 * cov + c2)) /
            ((basicMidSq + compressedMidSq + c1) * (basicDisp + compressedDisp + c2));

}

QString MainWindow::getFrameAssessmentValue(size_t currTime) {
    generateTempFrameFiles(currTime);
    QImage basicPicture;
    QImage compressedPicture;
    basicPicture.load(TMP_IMAGE_BASIC_FILENAME);
    compressedPicture.load(TMP_IMAGE_COMPRESSED_FILENAME);

    if (ui->selectedMethodMathExpecting->isChecked()) {
        QString val = QString::number(frameAssessmentMathExpecting(basicPicture, compressedPicture));
        val.replace(".", DOUBLE_NUM_SEPARATOR);
        return QString::number(currTime) + COL_SEPARATOR +
                val + ROW_SEPARATOR;
    }

    if (ui->selectedMethodSup->isChecked()) {
        QString val = QString::number(frameAssessmentSup(basicPicture, compressedPicture));
        val.replace(".", DOUBLE_NUM_SEPARATOR);
        return QString::number(currTime) + COL_SEPARATOR +
                val + ROW_SEPARATOR;
    }

    if (ui->selectedMethodPSNR->isChecked()) {
        QString val = QString::number(frameAssessmentPSNR(basicPicture, compressedPicture));
        val.replace(".", DOUBLE_NUM_SEPARATOR);
        return QString::number(currTime) + COL_SEPARATOR +
                val + ROW_SEPARATOR;
    }

    if (ui->selectedMethodSSIM->isChecked()) {
        QString val = QString::number(frameAssessmentSSIM(basicPicture, compressedPicture));
        val.replace(".", DOUBLE_NUM_SEPARATOR);
        return QString::number(currTime) + COL_SEPARATOR +
                val + ROW_SEPARATOR;
    }
    return "";
}

QString MainWindow::getCSVOutputData() {
    size_t videoLength = ui->videoLength->text().toInt();
    QString res = "Time" + COL_SEPARATOR + "Value" + ROW_SEPARATOR;
    for (size_t currTime = 0; currTime < videoLength; currTime++) {
        res += getFrameAssessmentValue(currTime);
        ui->progressBar->setValue(getProgressValue(currTime, videoLength));
    }
    removeTempFrameFiles();
    return res;
}

void MainWindow::on_basicVideoFileSelectBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(0, "Open", "", SUPPORTED_FILE_TYPES);
    ui->basicVideoFilePath->setText(filePath);
}

void MainWindow::on_compressedVideoFileSelectBtn_2_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(0, "Open", "", SUPPORTED_FILE_TYPES);
    ui->compressedVideoFilePath->setText(filePath);
}

void MainWindow::on_calculate_clicked()
{
    if (!isAnyMethodSelected()) {
        QMessageBox::about(this, "Status", "No method selected");
        return;
    }
    setEditableElementsDisabled(true);
    ui->progressBar->setValue(0);
    //setCursor(Qt::WaitCursor);
    QFile fout(ui->resultSavePath->text());

    if (fout.open(QIODevice::ReadWrite)) {
        QTextStream stream(&fout);
        stream << getCSVOutputData();
        //setCursor(Qt::ArrowCursor);
        ui->progressBar->setValue(100);
        QMessageBox::about(this, "Status", "Successfully completed");
    } else {
        //setCursor(Qt::ArrowCursor);
        QMessageBox::about(this, "Status", "Saving error");
    }
    setEditableElementsDisabled(false);
}

void MainWindow::on_resultSaveSelectBtn_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(0, "Save", "result.csv", "*.csv");
    ui->resultSavePath->setText(filePath);
}
