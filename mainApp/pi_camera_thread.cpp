#include "pi_camera_thread.h"
#include <qdebug.h>
#include <QDateTime>
#include <QLabel>

pi_camera_thread::pi_camera_thread(QObject *parent)
    : QThread{parent}
{
    thread_is_stop = false;
}

void pi_camera_thread::run()
{
    const std::string videoStreamAddress = "http://192.168.0.200:5000/video_feed";
    // check if we succeeded
    qDebug() << "called";
    if (!cap.open(videoStreamAddress)) {
        cerr << "ERROR! Unable to open camera\n";
        return;
    }
    for(;;)
    {
        if(thread_is_stop)
            break;
//        QLabel *child = new QLabel;
        cap.read(frame);
        resize(frame, frame, cv::Size(640,480));
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QPixmap pix = QPixmap::fromImage(QImage((unsigned char*) frame.data, frame.cols, frame.rows, QImage::Format_RGB888));
//        int time = QDateTime::currentMSecsSinceEpoch();
//        child->setPixmap(pix);
//        child->show();
        emit send_set_pixmap_label(pix);

//        FPS = 1000/(time - pre_time);

//        pre_time = time;
//        qDebug() << FPS;
//        qDebug() << "called";

    }
    return;
}

void pi_camera_thread::stop()
{
    thread_is_stop = true;
}
