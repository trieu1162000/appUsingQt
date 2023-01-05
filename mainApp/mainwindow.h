#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMath>
#include <QButtonGroup>
#include <QMessageBox>
#include <QSize>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QElapsedTimer>
#include <QTableWidgetItem>
//#include <opencv2/opencv.hpp>
#include "qdatetime.h"
#include "setting_com_window.h"
#include "nodes_info.h"
#include "nodes.h"
#include "camera.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    Ui::MainWindow *ui;
    ~MainWindow();

private slots:
    void on_setting_pushButton_clicked();
    void receive_is_connected_from_main(int);
    void on_run_button_clicked();
    void receive_data_from_sc(QList<uint8_t>);
    void show_back();

    void on_pause_resume_button_clicked();

    void update_loop();
    void handle_task_loop();
    void handle_run_back_loop();

    void on_nodes_i4_pushButton_clicked();

    void on_add_node_pushButton_clicked();

    void on_ok_node_pushButton_clicked();

    void on_clear_path_pushButton_clicked();

    void on_manual_mode_button_clicked();

    void receive_direction(int);
    void receive_control_pump(int);

signals:
    void send_path_run_signal(QList<QString>);
    void send_path_run_back_signal(QList<QString>);
    void send_mode_signal(int);
    void send_direction_signal(int);
    void send_control_pump_signal(int);
    void send_start_task_node_signal(int);
    void send_battery_info(int);
    void send_water_info(int);

private:
    setting_com_window *sc_window;
    camera *camera_window;
    nodes_info *node_i4_window;
    int sc_is_created = 0;
    int nodei4_is_created = 0;
    QString robot_info;

    // robot info
    uint8_t robot_battery = 0;
    uint8_t robot_speed = 0;
    uint8_t robot_water_capacity = 0;

    // auto mode info
    uint8_t start_task_node;
    uint8_t robot_current_node = 15;
    uint8_t robot_current_orient = 'N';
    uint8_t obstacle_distance = 0;
    int all_task_complete = 0; // 0 is true, >=1 is false
    int task_complete_flag = 0; // 0 is true, 1 is false
    int back_complete_flag = 0; // 0 is true, 1 is false
    typedef struct{
        int task_id;
        bool is_completed;
        bool is_running;
    } task;
    QList<task> task_to_do;


    int robot_mode = 0; // 0 is stop and 1 is running

    QTime time;
    QVector<QString> path_run, path_run_task, path_run_back, path_chose;
    QGraphicsScene *scene;
    QTableWidgetItem *status_item;
    QTableWidgetItem *velocity_item;
    QTableWidgetItem *current_orient_item;
    QTableWidgetItem *current_node_item;
    QTableWidgetItem *time_stop_item;
    QTableWidgetItem *time_start_item;

    void create_map();
    void update_map();
    bool solve_AStar();
    void draw_connections();
    void draw_nodes();
    QVector<QString> draw_path(QVector<QString>);
    void draw_task_path(QVector<QString>);
    void renderScene();
    QVector<QString> create_path_run(QVector<QString>);
    bool check_path(QString, QString);
    QVector<QVector<char>> create_orient_matrix();

    const QSize UNIT_SIZE   = QSize(5  , 5  );
    const QSize WINDOW_SIZE = QSize(135, 135);
    const QSize SCENE_SIZE  = QSize(UNIT_SIZE.width()*WINDOW_SIZE.width(), UNIT_SIZE.height()*WINDOW_SIZE.height());

    QList<my_Node*> nodes_in_path;
    int nMapWidth = 5;
    int nMapHeight = 3;
    int flag_run_back = 0;
    int flag_ACK_task = 0;
    int flag_ACK_back = 0;
    int flag_complete_back = 0;

    int m_mousePosX = 0;
    int m_mousePosY = 0;
    bool run_btn_clicked = false;
    bool m_shiftPressed = false;
    bool m_controlPressed = false;

    int FPS           = 60;
    float m_deltaTime = 0.0f;
    float m_loopTime  = 1000;
    const float m_loopSpeed = int(1000.0f/FPS);
    QTimer m_timer;
    QTimer handle_task_timer;
    QTimer handle_run_back_timer;
    QElapsedTimer m_elapsedTimer;

    my_Node* nodes = nullptr;
    my_Node* node_start = nullptr;
    my_Node* node_end = nullptr;

    QVector<QVector<char>> orient_matrix;

};
#endif // MAINWINDOW_H
