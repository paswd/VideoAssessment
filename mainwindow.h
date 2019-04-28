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

private slots:
    void on_basicVideoFileSelectBtn_clicked();

    void on_compressedVideoFileSelectBtn_2_clicked();

    void on_calculate_clicked();

    void on_resultSaveSelectBtn_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
