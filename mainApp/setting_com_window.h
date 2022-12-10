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
//    QString code;
//    int codeSize;

private slots:
    void on_connect_pushButton_clicked();
    void on_ok_pushButton_clicked();
    void receive_message();
signals:
    void sc_send_is_connected(int);
    void pass_data_to_main(QString);

private:
    Ui::setting_com_window *ui;
};

#endif // SETTING_COM_WINDOW_H
