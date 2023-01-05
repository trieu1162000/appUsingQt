#ifndef CAMERA_THREAD_H
#define CAMERA_THREAD_H

#include <QThread>
#include <QPixmap>
#include <QImage>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

class camera_thread : public QThread
{
    Q_OBJECT
public:
    explicit camera_thread(QObject *parent = nullptr);
    void stop();

private:
    VideoWriter *video;
    Mat frame;
    VideoCapture cap;
    bool thread_is_stop;

protected:
    void run();

signals:
    void send_set_pixmap_label(QPixmap);

};

#endif // CAMERA_THREAD_H
