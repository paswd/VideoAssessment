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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString getVideoFrameCommand(QString fileIn, QString fileOut, QString timestamp);
    void generateTempFrameFiles(size_t currTime);
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
