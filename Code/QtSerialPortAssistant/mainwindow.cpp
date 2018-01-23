#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPortInfo>
#include <QDebug>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->textEdit_recv->setReadOnly(true);

    m_recv_count = 0;
    m_send_count = 0;

    scan_available_serial_port();
    ui->comboBox_port->setCurrentIndex(0);
    connect(ui->comboBox_port, SIGNAL(pressed()),
            this, SLOT(scan_available_serial_port()));
    connect(ui->pushButton_open, SIGNAL(clicked(bool)),
            this, SLOT(serial_port_open_button_click_slot()));
    connect(&m_serial_port, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
            this, SLOT(serial_port_error_slot(QSerialPort::SerialPortError)));
    connect(&m_serial_port, SIGNAL(readyRead()),
            this, SLOT(serial_port_recv_data_slot()));

    connect(ui->checkBox_recv_in_hex, SIGNAL(stateChanged(int)),
            this, SLOT(checkbox_recv_in_hex_change_slot()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::scan_available_serial_port(void)
{
     QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();

     ui->comboBox_port->clear();

     if(infos.isEmpty())
         return;

     foreach (QSerialPortInfo info, infos) {
         ui->comboBox_port->addItem(info.portName());
     }

}

void MainWindow::serial_port_open_button_click_slot()
{
    if(ui->pushButton_open->text() == "打开")
    {
        if(m_serial_port.isOpen())
        {
            ui->pushButton_open->setText("关闭");
        }
        else
        {
            m_serial_port.setPortName(ui->comboBox_port->currentText());
            m_serial_port.setBaudRate(ui->comboBox_baud->currentText().toInt());
            if(ui->comboBox_databit->currentText() == "8")
                m_serial_port.setDataBits(QSerialPort::Data8);
            else if(ui->comboBox_databit->currentText() == "7")
                m_serial_port.setDataBits(QSerialPort::Data7);
            else if(ui->comboBox_databit->currentText() == "6")
                m_serial_port.setDataBits(QSerialPort::Data6);
            else if(ui->comboBox_databit->currentText() == "5")
                m_serial_port.setDataBits(QSerialPort::Data5);

            if(ui->comboBox_stopbit->currentText() == "1")
                m_serial_port.setStopBits(QSerialPort::OneStop);
            else if(ui->comboBox_stopbit->currentText() == "2")
                m_serial_port.setStopBits(QSerialPort::TwoStop);
            else if(ui->comboBox_stopbit->currentText() == "1.5")
                m_serial_port.setStopBits(QSerialPort::OneAndHalfStop);

            if(ui->comboBox_check->currentText() == "NONE")
                m_serial_port.setParity(QSerialPort::NoParity);
            else if(ui->comboBox_check->currentText() == "EVEN")
                m_serial_port.setParity(QSerialPort::EvenParity);
            else if(ui->comboBox_check->currentText() == "ODD")
                m_serial_port.setParity(QSerialPort::OddParity);

            if(m_serial_port.open(QSerialPort::ReadWrite))
            {
                ui->pushButton_open->setText("关闭");
                ui->comboBox_port->setEnabled(false);
                ui->comboBox_baud->setEnabled(false);
                ui->comboBox_databit->setEnabled(false);
                ui->comboBox_stopbit->setEnabled(false);
                ui->comboBox_check->setEnabled(false);
            }
            else
            {

            }
        }
    }
    else
    {
        if(m_serial_port.isOpen())
        {
            m_serial_port.close();
        }
        ui->pushButton_open->setText("打开");
        ui->comboBox_port->setEnabled(true);
        ui->comboBox_baud->setEnabled(true);
        ui->comboBox_databit->setEnabled(true);
        ui->comboBox_stopbit->setEnabled(true);
        ui->comboBox_check->setEnabled(true);
    }
}

void MainWindow::serial_port_error_slot(QSerialPort::SerialPortError error)
{
    if(m_serial_port.isOpen())
    {
        m_serial_port.close();
    }

    ui->comboBox_port->setEnabled(true);
    ui->comboBox_baud->setEnabled(true);
    ui->comboBox_databit->setEnabled(true);
    ui->comboBox_stopbit->setEnabled(true);
    ui->comboBox_check->setEnabled(true);
    ui->pushButton_open->setText("打开");
    ui->pushButton_send->setText("发送");
}

QString MainWindow::bytearray2hexstring(QByteArray &array)
{
    int length = array.length();
    uchar* uch = (uchar*)array.data();
    QString string;
    for(int i=0; i<length; i++)
    {
        string += QString("%1 ").arg(uch[i], 2, 16, QChar('0')).toUpper();
    }
    return string;
}

void MainWindow::serial_port_recv_data_slot()
{
    QByteArray recv_array = m_serial_port.readAll();

    m_current_string += QString::fromLocal8Bit(recv_array);
    m_current_hex_string += bytearray2hexstring(recv_array);

    m_recv_count += recv_array.length();
    ui->groupBox_recv->setTitle(QString("接收(%1)").arg(m_recv_count));

    if(m_current_string.length() >= STRING_MAX_LENGTH)
    {
        m_recv_string_list.append(m_current_string);
        m_recv_string_hex_list.append(m_current_hex_string);
        m_current_string = QString("");
        m_current_hex_string = QString("");
    }

    if(ui->checkBox_recv_in_hex->isChecked())
    {
        if(m_recv_string_list.isEmpty())
        {
            ui->textEdit_recv->setText(m_current_hex_string);
        }
        else
        {
            ui->textEdit_recv->setText(m_recv_string_hex_list.at(
                                           m_recv_string_hex_list.length()-1) +
                                       m_current_hex_string);
        }
    }
    else
    {
        if(m_recv_string_list.isEmpty())
        {
            ui->textEdit_recv->setText(m_current_string);
        }
        else
        {
            ui->textEdit_recv->setText(m_recv_string_list.at(
                                           m_recv_string_list.length()-1) +
                                       m_current_string);
        }
    }

    QScrollBar* scrollbar = ui->textEdit_recv->verticalScrollBar();
    if(scrollbar)
    {
        scrollbar->setSliderPosition(scrollbar->maximum());
    }
}

void MainWindow::checkbox_recv_in_hex_change_slot()
{
    if(ui->checkBox_recv_in_hex->isChecked())
    {
        if(m_recv_string_list.isEmpty())
        {
            ui->textEdit_recv->setText(m_current_hex_string);
        }
        else
        {
            ui->textEdit_recv->setText(m_recv_string_hex_list.at(
                                           m_recv_string_hex_list.length()-1) +
                                       m_current_hex_string);
        }
    }
    else
    {
        if(m_recv_string_list.isEmpty())
        {
            ui->textEdit_recv->setText(m_current_string);
        }
        else
        {
            ui->textEdit_recv->setText(m_recv_string_list.at(
                                           m_recv_string_list.length()-1) +
                                       m_current_string);
        }
    }
}

void MainWindow::on_pushButton_recv_clear_clicked()
{
    m_recv_string_hex_list.clear();
    m_recv_string_list.clear();
    m_current_hex_string.clear();
    m_current_string.clear();
    ui->textEdit_recv->setText("");
}

void MainWindow::on_pushButton_recount_clicked()
{
    m_recv_count = 0;
    m_send_count = 0;

    ui->groupBox_recv->setTitle(QString("接收(%1)").arg(m_recv_count));
    ui->groupBox_send->setTitle(QString("发送(%1)").arg(m_send_count));
}

void MainWindow::on_pushButton_send_clear_clicked()
{
    ui->textEdit_send->setText("");
}

void MainWindow::on_pushButton_send_clicked()
{
    if(ui->pushButton_send->text() == "发送")
    {
        if(!m_serial_port.isOpen())
            return;
        if(ui->textEdit_send->toPlainText().isEmpty())
            return;

        send_text_content();

        if(ui->checkBox_continue->isChecked())
        {
            ui->pushButton_send->setText("停止");
            m_timer.singleShot(0, this, SLOT(serial_port_timer_send_slot()));
        }
        else if(ui->checkBox_timer->isChecked())
        {
            bool t_check;
            int t = ui->lineEdit->text().toInt(&t_check);
            if(!t_check)
                return;
            ui->pushButton_send->setText("停止");
            m_timer.singleShot(t, this, SLOT(serial_port_timer_send_slot()));
        }
    }
    else
    {
        ui->pushButton_send->setText("发送");
    }
}

void MainWindow::serial_port_timer_send_slot()
{
    if(ui->pushButton_send->text() == "停止")
    {
        if(!m_serial_port.isOpen() || ui->textEdit_send->toPlainText().isEmpty())
        {
            ui->pushButton_send->setText("发送");
            return;
        }

        send_text_content();

        if(ui->checkBox_continue->isChecked())
        {
            m_timer.singleShot(0, this, SLOT(serial_port_timer_send_slot()));
        }
        else if(ui->checkBox_timer->isChecked())
        {
            bool t_check;
            int t = ui->lineEdit->text().toInt(&t_check);
            if(!t_check)
            {
                ui->pushButton_send->setText("发送");
                return;
            }
            m_timer.singleShot(t, this, SLOT(serial_port_timer_send_slot()));
        }
    }
    else
    {

    }
}

void MainWindow::send_text_content()
{
    QString  s = ui->textEdit_send->toPlainText();
    QByteArray sb;
    if(ui->checkBox_send_in_hex->isChecked())
    {
        sb = hexstring2bytearray(s);
    }
    else
    {
        sb = s.toLocal8Bit();
    }
    int count = m_serial_port.write(sb.data(), sb.length());
    m_send_count += count;
    ui->groupBox_send->setTitle(QString("发送(%1)").arg(m_send_count));
}

QByteArray MainWindow::hexstring2bytearray(QString s)
{
    QByteArray ba;

    QStringList sl = s.split(" ", QString::SkipEmptyParts);

    for(int i=0; i<sl.length(); i++)
    {
        QString si = sl.at(i);
        if(si.length() >= 3)
        {
            return QByteArray();
        }
        bool result;
        char value = si.toInt(&result, 16);
        if(result)
        {
            ba.append(value);
        }
        else
        {
            return QByteArray();
        }
    }

    return ba;
}
