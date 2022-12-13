#ifndef SETTING_COM_WINDOW_H
#define SETTING_COM_WINDOW_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>

namespace Ui {
class setting_com_window;
}

class setting_com_window : public QDialog
{
    Q_OBJECT

public:
    explicit setting_com_window(QWidget *parent = nullptr);
    ~setting_com_window();
    int sc_is_connected = 0;
    QSerialPort serialPort;
    QSerialPortInfo info;
    QString buffer;
    QString portName;
//    QString code;
//    int codeSize;

private slots:
    void on_connect_pushButton_clicked();
    void receive_message();
    void send_path_run(QList<QString>);
    void send_path_run_back(QList<QString>);
    void send_mode(int);
    void send_direction(int);
    void send_start_task_node(int);

signals:
    void sc_send_is_connected(int);
    void pass_data_to_main(QList<std::byte>);

private:
    Ui::setting_com_window *ui;
    QList<std::byte> bytes_received;
    QList<std::byte> main_bytes_received;
    int start_index;
    int stop_index;
};

#endif // SETTING_COM_WINDOW_H
