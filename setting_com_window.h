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

private:
    Ui::setting_com_window *ui;
    QSerialPort serialPort;
    QSerialPortInfo info;
    QString buffer;
    QString code;
    int codeSize;
};

#endif // SETTING_COM_WINDOW_H
