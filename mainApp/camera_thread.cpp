#include "camera_thread.h"

camera_thread::camera_thread(QObject *parent) :
    QThread(parent)
{
    thread_is_stop = false;
}

void camera_thread::run()
{
    cap.open(qPrintable("http://192.168.1.200:5000/"));
    // check if we succeeded
    if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return;
    }
    for(;;)
    {
        if(thread_is_stop )
            break;
        cap.read(frame);
        resize(frame, frame, cv::Size(640,480));
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QPixmap pix = QPixmap::fromImage(QImage((unsigned char*) frame.data, frame.cols, frame.rows, QImage::Format_RGB888));
        emit send_set_pixmap_label(pix);

    }
    return;
}

void camera_thread::stop()
{
    thread_is_stop = true;
}
