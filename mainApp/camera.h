#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>
#include "pi_camera_thread.h"

namespace Ui {
class camera;
}

class camera : public QWidget
{
    Q_OBJECT

public:
    explicit camera(QWidget *parent = nullptr);
    ~camera();

private slots:
    void on_quit_pushButton_clicked();
    void receive_battery_info(int);
    void receive_water_info(int);

    void on_left_pushButton_pressed();

    void on_down_pushButton_pressed();

    void on_right_pushButton_pressed();

    void on_up_pushButton_pressed();

    void on_connect_pushButton_clicked();

    void on_radioButton_toggled(bool checked);

    void on_left_pushButton_released();

    void on_up_pushButton_released();

    void on_down_pushButton_released();

    void on_right_pushButton_released();

//    void set_pixmap_label(QPixmap);

signals:
    void send_show_back_signal();
    void send_direction(int);
    void send_control_pump(int);

private:
    Ui::camera *ui;
    pi_camera_thread *thread_camera;
    bool camera_is_connected;
};

#endif // CAMERA_H
