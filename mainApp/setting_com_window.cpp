#include "setting_com_window.h"
#include "ui_setting_com_window.h"

setting_com_window::setting_com_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setting_com_window)
{
    ui->setupUi(this);
    QDialog::setWindowTitle("COM Setting");
    ui->connected_label->hide();
    ui->ic_connected_label->hide();

    /* Ports */
    QList<QSerialPortInfo> ports = info.availablePorts();
    qDebug() << "Number of serial ports:" << ports.count();
    QList<QString> stringPorts;
    for(int i = 0 ; i < ports.size() ; i++){
        stringPorts.append(ports.at(i).portName());
    }
    ui->port_comboBox->addItems(stringPorts);
    /* Baud Rate Ratios */
    QList<qint32> baudRates = info.standardBaudRates(); // What baudrates does my computer support ?
    QList<QString> stringBaudRates;
    for(int i = 0 ; i < baudRates.size() ; i++){
        if(baudRates.at(i)>=1200 && baudRates.at(i)<=19200)
            stringBaudRates.append(QString::number(baudRates.at(i)));
    }
    ui->brate_comboBox->addItems(stringBaudRates);
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

    ui->brate_comboBox->setCurrentIndex(3);
    ui->dtbits_comboBox->setCurrentIndex(3);

//    header_size = header.size();
//    EOF_size = EOFrame.size();

}

setting_com_window::~setting_com_window()
{
    delete ui;
}

void setting_com_window::on_connect_pushButton_clicked()
{
    ui->connected_label->show();
    ui->ic_connected_label->show();
    portName = ui->port_comboBox->currentText();
    serialPort.setPortName(portName);
    serialPort.open(QIODevice::ReadWrite);
//    if(sc_is_connected == 1)
//    {
//        // Change button to disconnect and
//        ui->connect_pushButton->setText("Disconnect");
//        QPixmap pixmap(":/imgs/icon_image/icon_disconnect.png");
//        QIcon ButtonIcon(pixmap);
//        ui->connect_pushButton->setIcon(ButtonIcon);
//        ui->connect_pushButton->setIconSize(pixmap.rect().size());

//        // Label will be connected
//        ui->connected_label->setStyleSheet("QLabel {color : green}");
//        ui->connected_label->setText(QString(portName) + QString(" is connected"));
//        QPixmap pixmap1(":/imgs/icon_image/icon_connect.png");
//        ui->ic_connected_label->setPixmap(pixmap1);
//    }
    if(ui->connect_pushButton->text()=="Connect"){
        if(!serialPort.isOpen()){
            ui->connected_label->setStyleSheet("QLabel {color : darkred}");
            if(portName == nullptr)
                ui->connected_label->setText("No PORT available");
            else
                ui->connected_label->setText(QString("Can not connect to ") + QString(portName));
            QPixmap pixmap(":/imgs/icon_image/icon_disconnect.png");
            ui->ic_connected_label->setPixmap(pixmap);
            sc_is_connected = 0;
            emit sc_send_is_connected(sc_is_connected);

        }
        else
        {
            QString stringbaudRate = ui->brate_comboBox->currentText();
            int intbaudRate = stringbaudRate.toInt();
            serialPort.setBaudRate(intbaudRate);

            QString dataBits = ui->dtbits_comboBox->currentText();
            if(dataBits == "5 Bits") {
               serialPort.setDataBits(QSerialPort::Data5);
            }
            else if((dataBits == "6 Bits")) {
               serialPort.setDataBits(QSerialPort::Data6);
            }
            else if(dataBits == "7 Bits") {
               serialPort.setDataBits(QSerialPort::Data7);
            }
            else if(dataBits == "8 Bits"){
               serialPort.setDataBits(QSerialPort::Data8);
            }

            QString stopBits = ui->sbits_comboBox->currentText();
            if(stopBits == "1 Bit") {
             serialPort.setStopBits(QSerialPort::OneStop);
            }
            else if(stopBits == "1,5 Bits") {
             serialPort.setStopBits(QSerialPort::OneAndHalfStop);
            }
            else if(stopBits == "2 Bits") {
             serialPort.setStopBits(QSerialPort::TwoStop);
            }

            QString parity = ui->parity_comboBox->currentText();
            if(parity == "No Parity"){
              serialPort.setParity(QSerialPort::NoParity);
            }
            else if(parity == "Even Parity"){
              serialPort.setParity(QSerialPort::EvenParity);
            }
            else if(parity == "Odd Parity"){
              serialPort.setParity(QSerialPort::OddParity);
            }
            serialPort.setFlowControl(QSerialPort::NoFlowControl);
            connect(&serialPort,SIGNAL(readyRead()),this,SLOT(receive_message()));
            // Change button to disconnect and
            ui->connect_pushButton->setText("Disconnect");
            QPixmap pixmap(":/imgs/icon_image/icon_disconnect.png");
            QIcon ButtonIcon(pixmap);
            ui->connect_pushButton->setIcon(ButtonIcon);
            ui->connect_pushButton->setIconSize(pixmap.rect().size());

            // Label will be connected
            ui->connected_label->setStyleSheet("QLabel {color : green}");
            ui->connected_label->setText(QString(portName) + QString(" is connected"));
            QPixmap pixmap1(":/imgs/icon_image/icon_connect.png");
            ui->ic_connected_label->setPixmap(pixmap1);
            sc_is_connected = 1;
            emit sc_send_is_connected(sc_is_connected);
        }
    }
    else
    {
        // Disconnect Serial Port and label will be disconnected
        serialPort.close();
        // Change button to connect
        ui->connect_pushButton->setText("Connect");
        QPixmap pixmap(":/imgs/icon_image/icon_connect.png");
        QIcon ButtonIcon(pixmap);
        ui->connect_pushButton->setIcon(ButtonIcon);
        ui->connect_pushButton->setIconSize(pixmap.rect().size());
        ui->connected_label->setStyleSheet("QLabel {color : darkred}");
        ui->connected_label->setText(QString(portName) + QString(" is disconnected"));
        QPixmap pixmap1(":/imgs/icon_image/icon_disconnect.png");
        ui->ic_connected_label->setPixmap(pixmap1);
        sc_is_connected = 0;
        emit sc_send_is_connected(sc_is_connected);
    }
}

void setting_com_window::receive_message()
{
    QByteArray data_BA = serialPort.readAll();
//    QByteArray data_BA(raw_data_BA.simplified());
//    qDebug() << data_BA.size() << data_BA.toHex();
    uint8_t byte = data_BA.at(0);
    bytes_received.append(byte);
//    qDebug() << bytes_received;
    if(bytes_received.size() < 3)
        return;
    uint length = bytes_received.size();
    if(bytes_received[length -2] != 0xAE && bytes_received[length - 3] != 0xAE)
        return;


    qDebug() << bytes_received;
//    uint8_t byte = data_BA.at(0);
//    qDebug() << byte;
//    if(data_BA.at(0) == (QChar)0xEE)
//        qDebug() << "true";
//    if(data_BA.size()<8)
//        return;
//    QString data(data_BA);
//    buffer.append(data);
//    int index_header = buffer.indexOf(header);
//    int index_EOF = buffer.indexOf(EOFrame);
//    QDataStream stream(&data_BA, QIODevice::ReadWrite);
//    stream >> bytes_received;

//    qDebug() << bytes_received;

//    qDebug() << bytes_receive[0];
//    qDebug() << bytes_receive[1];

    for(int i = 0; i <bytes_received.size(); i++)
    {
//        qDebug() << bytes_receive[i];
        if(bytes_received[i] ==  0xAA  )
        {

            start_index = i+1;
            break;
        }

    }
    stop_index = bytes_received.size() - 2;
    for(int i = start_index; i < stop_index; i++)
    {
        main_bytes_received.append(bytes_received.at(i));

    }

    qDebug() <<main_bytes_received;
    emit pass_data_to_main(main_bytes_received);
    main_bytes_received.clear();
    bytes_received.clear();

}

void setting_com_window::send_path_run(QList<QString> path)
{
    qDebug() << path;
    QByteArray data_to_send;
    for(int i = 0; i<path.size(); i= i+2)
    {
        if(path[i].toInt() == 0)
            data_to_send.append(path[i].toInt()+16);
        else
            data_to_send.append(path[i].toInt());
        data_to_send.append(path[i+1].toUtf8());
    }

    data_to_send.prepend(0xC0);
    data_to_send.prepend(0xEE);
    data_to_send.prepend(0xAA);
    data_to_send.append(0xAE);
    data_to_send.append(0xAE);
    qDebug() << data_to_send;
    qDebug() << data_to_send.size();
    serialPort.write(data_to_send);
}

void setting_com_window::send_path_run_back(QList<QString> path)
{
    qDebug() << path;
    QByteArray data_to_send;
    for(int i = 0; i<path.size(); i= i+2)
    {
        if(path[i].toInt() == 0)
            data_to_send.append(path[i].toInt()+16);
        else
            data_to_send.append(path[i].toInt());
        data_to_send.append(path[i+1].toUtf8());
    }

    data_to_send.prepend(0xC1);
    data_to_send.prepend(0xEE);
    data_to_send.prepend(0xAA);
    data_to_send.append(0xAE);
    data_to_send.append(0xAE);
//    qDebug() << data_to_send;
//    qDebug() << data_to_send.size();
    serialPort.write(data_to_send);
}

void setting_com_window::send_start_task_node(int data)
{
    QByteArray data_to_send;
    data_to_send.append(data);

    data_to_send.prepend(0xC2);
    data_to_send.prepend(0xEE);
    data_to_send.prepend(0xAA);
    data_to_send.append(0xAE);
    data_to_send.append(0xAE);
    qDebug() << data_to_send;
    qDebug() << data_to_send.size();
    serialPort.write(data_to_send);
}

void setting_com_window::send_mode(int data)
{
    QByteArray data_to_send;
    data_to_send.append(data);
    data_to_send.prepend(0xB0);
    data_to_send.prepend(0xEE);
    data_to_send.prepend(0xAA);
    data_to_send.append(0xAE);
    data_to_send.append(0xAE);
    qDebug() << data_to_send;
    qDebug() << data_to_send.size();
    serialPort.write(data_to_send);
}

void setting_com_window::send_control_pump(int data)
{
    QByteArray data_to_send;
    data_to_send.append(data);
    data_to_send.prepend(0xD0);
    data_to_send.prepend(0xEE);
    data_to_send.prepend(0xAA);
    data_to_send.append(0xAE);
    data_to_send.append(0xAE);
    qDebug() << data_to_send;
    qDebug() << data_to_send.size();
    serialPort.write(data_to_send);
}

void setting_com_window::send_direction(int data)
{
    QByteArray data_to_send;
    data_to_send.append(data);
    data_to_send.prepend(0xD1);
    data_to_send.prepend(0xEE);
    data_to_send.prepend(0xAA);
    data_to_send.append(0xAE);
    data_to_send.append(0xAE);
    qDebug() << data_to_send;
    qDebug() << data_to_send.size();
    serialPort.write(data_to_send);
}







