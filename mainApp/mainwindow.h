#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QMessageBox>
#include <QSize>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QElapsedTimer>
#include "setting_com_window.h"
#include "nodes_info.h"
#include "nodes.h"

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
    void receive_data_from_sc(QString);
    void show_back();

    void on_pause_resume_button_clicked();

    void loop();

    void on_nodes_i4_pushButton_clicked();

private:
    setting_com_window *sc_window;
    nodes_info *node_i4_window;
    int sc_is_created = 0;
    int nodei4_is_created = 0;
    QString robot_info;
    QGraphicsScene *scene;
    int robot_status = 0; // 0 is stop and 1 is running
    void create_map();
    void update_map();
    bool solve_AStar();
    void draw_connections();
    void draw_nodes();
    void draw_path();
    void renderScene();

    const QSize UNIT_SIZE   = QSize(5  , 5  );
    const QSize WINDOW_SIZE = QSize(135, 135);
    const QSize SCENE_SIZE  = QSize(UNIT_SIZE.width()*WINDOW_SIZE.width(), UNIT_SIZE.height()*WINDOW_SIZE.height());

    QList<Node*> nodes_in_path;
    int nMapWidth = 5;
    int nMapHeight = 3;

    int m_mousePosX = 0;
    int m_mousePosY = 0;
    bool run_btn_clicked = false;
    bool m_shiftPressed = false;
    bool m_controlPressed = false;

    int FPS           = 60;
    float m_deltaTime = 0.0f;
    float m_loopTime  = 0.0f;
    const float m_loopSpeed = int(1000.0f/FPS);
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;

    Node* nodes = nullptr;
    Node* node_start = nullptr;
    Node* node_end = nullptr;
};
#endif // MAINWINDOW_H
