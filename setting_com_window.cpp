#include "setting_com_window.h"
#include "ui_setting_com_window.h"


setting_com_window::setting_com_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setting_com_window)
{
    ui->setupUi(this);
    QDialog::setWindowTitle("COM Setting");

    /* Ports */
    int i=0;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->port_comboBox->insertItem(i,info.portName());
        i++;
    }
//    QList<QSerialPortInfo> ports = info.availablePorts();
//    qDebug() << "Number of serial ports:" << ports.count();
//    QList<QString> stringPorts;
//    for(int i = 0 ; i < ports.size() ; i++){
//        stringPorts.append(ports.at(i).portName());
//    }
//    ui->port_comboBox->addItems(stringPorts);
//    // Baud Rate Ratios
//    QList<qint32> baudRates = info.standardBaudRates(); // What baudrates does my computer support ?
//    QList<QString> stringBaudRates;
//    for(int i = 0 ; i < baudRates.size() ; i++){
//        if(baudRates.at(i)>=1200 && baudRates.at(i)<=19200)
//            stringBaudRates.append(QString::number(baudRates.at(i)));
//    }
//    ui->brate_comboBox->addItems(stringBaudRates);
    i=0;
    foreach (const qint32 &info, QSerialPortInfo::standardBaudRates())
    {
        ui->brate_comboBox->insertItem(i,QString::number(info));
        i++;
    }

    /* Data Bits */
    ui->dtbits_comboBox->addItem("5");
    ui->dtbits_comboBox->addItem("6");
    ui->dtbits_comboBox->addItem("7");
    ui->dtbits_comboBox->addItem("8");

    // Stop Bits
    ui->sbits_comboBox->addItem("1 Bit");
    ui->sbits_comboBox->addItem("1,5 Bits");
    ui->sbits_comboBox->addItem("2 Bits");

    // Parities
    ui->parity_comboBox->addItem("No Parity");
    ui->parity_comboBox->addItem("Even Parity");
    ui->parity_comboBox->addItem("Odd Parity");

}

setting_com_window::~setting_com_window()
{
    delete ui;
}
