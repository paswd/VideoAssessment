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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QRgb>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setEditableElementsDisabled(bool disabled);
    bool isAnyMethodSelected();

    QString getVideoFrameCommand(QString fileIn, QString fileOut, QString timestamp);
    qreal getIntensity(QRgb pixel);

    void generateTempFrameFiles(size_t currTime);
    void removeTempFrameFiles();

    qreal rAbs(qreal v);
    qreal pixelDiff(QRgb a, QRgb b);

    int getProgressValue(size_t value, size_t maxValue);

    qreal frameAssessmentMathExpecting(QImage &basic, QImage &compressed);
    qreal frameAssessmentSup(QImage &basic, QImage &compressed);
    qreal frameAssessmentPSNR(QImage &basic, QImage &compressed);
    qreal ssimCalculateFrame(QImage &basic, QImage &compressed, size_t shiftHeight, size_t shiftWidth);
    qreal frameAssessmentSSIM(QImage &basic, QImage &compressed);

    QString getFrameAssessmentValue(size_t currTime);
    QString getCSVOutputData();

private slots:
    void on_basicVideoFileSelectBtn_clicked();

    void on_compressedVideoFileSelectBtn_2_clicked();

    void on_calculate_clicked();

    void on_resultSaveSelectBtn_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
