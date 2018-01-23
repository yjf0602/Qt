#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QStringList>
#include <QTimer>

#define STRING_MAX_LENGTH       5000

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString bytearray2hexstring(QByteArray &array);

public slots:
    void scan_available_serial_port(void);
    void serial_port_open_button_click_slot(void);
    void serial_port_error_slot(QSerialPort::SerialPortError error);
    void serial_port_recv_data_slot(void);

    void serial_port_timer_send_slot(void);

    void checkbox_recv_in_hex_change_slot(void);

private slots:
    void on_pushButton_recv_clear_clicked();

    void on_pushButton_recount_clicked();

    void on_pushButton_send_clear_clicked();

    void on_pushButton_send_clicked();

    void send_text_content(void);
    QByteArray hexstring2bytearray(QString s);

private:
    Ui::MainWindow *ui;

    QSerialPort m_serial_port;

    int m_recv_count;
    int m_send_count;

    QString m_current_string;
    QString m_current_hex_string;

    QStringList m_recv_string_list;
    QStringList m_recv_string_hex_list;

    QTimer m_timer;
};

#endif // MAINWINDOW_H
