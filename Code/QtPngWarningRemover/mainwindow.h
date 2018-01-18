#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pngresaver.h"
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void start_png_resaver(QString path);

private slots:
    void on_pushButton_dir_clicked();
    void on_pushButton_start_clicked();
    void process_slot(int percentage);

private:
    Ui::MainWindow *ui;
    QThread* m_th;
    PngResaver* m_png_resaver;
};

#endif // MAINWINDOW_H
