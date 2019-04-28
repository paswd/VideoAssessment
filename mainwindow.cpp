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
#include <QString>
#include <QFileDialog>
#include <QMessageBox>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

//Before starting install packages:
// * libavcodec-dev
// * libavformat-dev
// * libswscale-dev

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    av_register_all();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    QMessageBox::about(this, "Status", "Successfully completed");
}

void MainWindow::on_resultSaveSelectBtn_clicked()
{
    //QString filePath = QFileDialog::getOpenFileName(0, "Save", "", "*.bmp");
    QString filePath = QFileDialog::getSaveFileName(0, "Save", "result.bmp", "*.bmp");
    ui->compressedVideoFilePath->setText(filePath);
}
