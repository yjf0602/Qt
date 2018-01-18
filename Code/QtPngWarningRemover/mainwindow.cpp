#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_th = new QThread();
    m_th->start();
    m_png_resaver = new PngResaver();
    connect(m_png_resaver, SIGNAL(process_signal(int)),
            this, SLOT(process_slot(int)));
    connect(this, SIGNAL(start_png_resaver(QString)),
            m_png_resaver, SLOT(start_process(QString)));
    m_png_resaver->moveToThread(m_th);
}

MainWindow::~MainWindow()
{
    delete ui;
    m_th->quit();
    m_th->wait();
}

void MainWindow::on_pushButton_dir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory();

    ui->lineEdit_dir->setText(dir);
}

void MainWindow::on_pushButton_start_clicked()
{
    if(ui->pushButton_start->text() == "Start")
    {
        ui->pushButton_start->setText("Stop");
        emit start_png_resaver(ui->lineEdit_dir->text());
    }
    else
    {
        m_png_resaver->stop_process();
    }
}

void MainWindow::process_slot(int percentage)
{
    ui->progressBar->setValue(percentage);
    if(percentage == 100)
    {
        ui->pushButton_start->setText("Start");
    }
}

