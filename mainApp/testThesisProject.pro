QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera.cpp \
    main.cpp \
    mainwindow.cpp \
    nodes_info.cpp \
    pi_camera_thread.cpp \
    setting_com_window.cpp

HEADERS += \
    camera.h \
    mainwindow.h \
    nodes.h \
    nodes_info.h \
    pi_camera_thread.h \
    setting_com_window.h

FORMS += \
    camera.ui \
    mainwindow.ui \
    nodes_info.ui \
    setting_com_window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc


INCLUDEPATH += D:\Apps\opencv-4.6.0\opencv-4.6.0\build\install\include

LIBS += D:\Apps\opencv-4.6.0\opencv-4.6.0\build\bin\libopencv_core460.dll
LIBS += D:\Apps\opencv-4.6.0\opencv-4.6.0\build\bin\libopencv_calib3d460.dll
LIBS += D:\Apps\opencv-4.6.0\opencv-4.6.0\build\bin\libopencv_videoio460.dll
LIBS += D:\Apps\opencv-4.6.0\opencv-4.6.0\build\bin\libopencv_imgproc460.dll
LIBS += D:\Apps\opencv-4.6.0\opencv-4.6.0\build\bin\libopencv_imgcodecs460.dll
LIBS += D:\Apps\opencv-4.6.0\opencv-4.6.0\build\bin\libopencv_highgui460.dll
LIBS += D:\Apps\opencv-4.6.0\opencv-4.6.0\build\install\x64\mingw\bin\opencv_videoio_ffmpeg460_64.dll
