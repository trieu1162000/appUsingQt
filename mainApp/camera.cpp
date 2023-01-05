#include "camera.h"
#include "ui_camera.h"
#include <QShortcut>

camera::camera(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::camera)
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint );
    ui->setupUi(this);
    thread_camera = nullptr;
    camera_is_connected = false;
}

camera::~camera()
{
    delete ui;
}

void camera::on_quit_pushButton_clicked()
{
    emit send_show_back_signal();

    this->close();
}

void camera::receive_battery_info(int data)
{
//    qDebug() << "called";
    ui->battery_progressBar->setValue(data);
}
void camera::receive_water_info(int data)
{
//    qDebug() << "called";
    ui->water_progressBar->setValue(data);
}

// 0:stop, 1: up, 2: down, 3: left, 4: right
void camera::on_left_pushButton_pressed()
{
    emit send_direction(3);
    qDebug() << "called press";
}


void camera::on_down_pushButton_pressed()
{
    emit send_direction(2);
}


void camera::on_right_pushButton_pressed()
{
    emit send_direction(4);
}


void camera::on_up_pushButton_pressed()
{
    emit send_direction(1);
}


void camera::on_connect_pushButton_clicked()
{
    if(camera_is_connected == false)
    {
        thread_camera = new pi_camera_thread;
        camera_is_connected = true;
        thread_camera->start();
        connect(thread_camera, SIGNAL(send_set_pixmap_label(QPixmap)), ui->label, SLOT(setPixmap(QPixmap)));
        ui->connect_pushButton->setText("Disconnect");
        QPixmap pixmap(":/imgs/icon_image/icon_disconnect.png");
        QIcon ButtonIcon(pixmap);
        ui->connect_pushButton->setIcon(ButtonIcon);
        ui->connect_pushButton->setIconSize(pixmap.rect().size());

    }
    else
    {

        camera_is_connected = false;
        thread_camera->stop();
        disconnect(thread_camera, SIGNAL(send_set_pixmap_label(QPixmap)), ui->label, SLOT(setPixmap(QPixmap)));
        ui->connect_pushButton->setText("Connect");
        QPixmap pixmap(":/imgs/icon_image/icon_connect.png");
        QIcon ButtonIcon(pixmap);
        ui->connect_pushButton->setIcon(ButtonIcon);
        ui->connect_pushButton->setIconSize(pixmap.rect().size());
    }
}

void camera::on_left_pushButton_released()
{
    emit send_direction(0);
//    qDebug() << "called release";
}


void camera::on_up_pushButton_released()
{
    emit send_direction(0);
}


void camera::on_down_pushButton_released()
{
    emit send_direction(0);
}


void camera::on_right_pushButton_released()
{
    emit send_direction(0);
}

void camera::on_radioButton_toggled(bool checked)
{
    if(checked)
    {
        emit send_control_pump(1);
//        qDebug() << "true called";
    }
    else
//        qDebug() << "false called";
        emit send_control_pump(0);
}

//void camera::set_pixmap_label(QPixmap pix)
//{
//    ui->label->setPixmap(pix);
//}




