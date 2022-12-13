#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QDir>
#include <QPainter>

int receive_value_connected = 0;
MainWindow::MainWindow(QWidget *node_parent)
    : QMainWindow(node_parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pause_resume_button->setEnabled(false);
    connect(&m_timer, &QTimer::timeout, this, &MainWindow::loop);
    m_timer.start(m_loopTime);
    m_elapsedTimer.start();

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    node_i4_window = new nodes_info(this);
    node_i4_window->hide();
    qDebug() << node_i4_window->get_rows();
    ui->nodes_comboBox->addItems(node_i4_window->get_node_ids());
    create_map();
    nodes->node_count = node_i4_window->get_rows();
    orient_matrix = create_orient_matrix();
    int myInt = 0xFF;
    QByteArray bA;
    qDebug() << bA;
    QDataStream stream(&bA, QIODevice::WriteOnly);
    stream << myInt;
    qDebug() << bA;

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::create_path_run(QVector<QString> path)
{
    path.clear();
    for(int i = 0; i< path_run.size()-1; i++)
    {
        path.append(path_run[i]);
        path.append(QString((orient_matrix[path_run[i].toInt()][path_run[i+1].toInt()])));
    }
    qDebug() << path;
}

QVector<QVector<char>> MainWindow::create_orient_matrix()
{
    QVector<QVector<char>> matrix(nodes->node_count, QVector<char>(nodes->node_count));
//    qDebug() << nodes->node_count;
    for (int i = 0; i < nodes->node_count; i++)
    {
        QString n_nodes = node_i4_window->get_neighbour(i);
        QStringList n_nodes_list = n_nodes.split(", ");
        foreach (QString n_node, n_nodes_list)
        {
            int j = n_node.toInt();
            int vector[1][2];
            int dX = (nodes[j].x - nodes[i].x);
            int dY = (nodes[j].y - nodes[i].y);
            vector[0][0] = dX / (int)qSqrt(dX * dX + dY * dY);
            vector[0][1] = dY / (int)qSqrt(dX * dX + dY * dY);
            switch (vector[0][0])
            {
                case 0:
                    switch(vector[0][1])
                    {
                        case 1:
                            matrix[i][j] = 'S';
                            break;
                        case -1:
                            matrix[i][j] = 'N';
                            break;
                    }
                    break;
                case 1:
                    matrix[i][j] = 'E';
                    break;
                case -1:
                    matrix[i][j] = 'W';
                    break;
            }
        }
    }

    return matrix;
}


void MainWindow::create_map()
{
    nodes = new Node[node_i4_window->get_rows()];
    for(int i = 0; i < 16; i++){
        nodes[i].node_id = i;
        nodes[i].node_parent = nullptr;
        nodes[i].is_visited = false;
        nodes[i].is_obstacle = false;
        nodes[i].x = node_i4_window->get_x(i);
        nodes[i].y = node_i4_window->get_y(i);
        QString n_nodes = node_i4_window->get_neighbour(i);
        QStringList n_nodes_list = n_nodes.split(", ");
        foreach(QString n_node, n_nodes_list){
            nodes[i].nodes_neighbour.push_back(&nodes[n_node.toInt()]);
//            qDebug() << n_node.toInt();
        }

    }

    node_start = &nodes[15];
    ui->robot_label->setGeometry(nodes[15].x-10, nodes[15].y+45, 61, 31 );
    update_map();
}

void MainWindow::update_map()
{
    //drawing
    ui->robot_label->setGeometry(nodes[robot_current_node].x-10, nodes[robot_current_node].y+45, 61, 31 );
//    ui->
    scene->clear();
    draw_connections();
    draw_nodes();
    draw_path();
}

bool MainWindow::solve_AStar()
{
    for(int i = 0; i <16; ++i)
    {
        nodes[i].is_visited    = false;
        nodes[i].fGlobalGoal = INFINITY;
        nodes[i].fLocalGoal  = INFINITY;
        nodes[i].node_parent      = nullptr;
    }

    auto distance = [](Node* a, Node* b)
    {
        return sqrtf((a->x - b->x)*(a->x - b->x) + (a->y - b->y)*(a->y - b->y));
    };

    auto heuristic = [distance](Node* a, Node* b)
    {
        return distance(a, b);
    };

    Node* nodeCurrent = node_start;
    node_start->fLocalGoal = 0.0f;
    node_start->fGlobalGoal = heuristic(node_start, node_end);

    QList<Node*> listNotTestedNodes;
    listNotTestedNodes.push_back(node_start);

    while(!listNotTestedNodes.empty() && nodeCurrent != node_end)
    {
        std::sort(listNotTestedNodes.begin(), listNotTestedNodes.end(), [](const Node* lhs, const Node* rhs){ return lhs->fGlobalGoal < rhs->fGlobalGoal;});

        while(!listNotTestedNodes.empty() && listNotTestedNodes.front()->is_visited)
        {
            listNotTestedNodes.pop_front();
        }

        if(listNotTestedNodes.empty())
            break;

        nodeCurrent = listNotTestedNodes.front();
        nodeCurrent->is_visited = true;

        for(auto nodeNeighbour : nodeCurrent->nodes_neighbour)
        {
            if(!nodeNeighbour->is_visited && nodeNeighbour->is_obstacle == 0)
            {
                listNotTestedNodes.push_back(nodeNeighbour);
            }

            float fPossiblyLowerGoal = nodeCurrent->fLocalGoal + distance(nodeCurrent, nodeNeighbour);

            if(fPossiblyLowerGoal < nodeNeighbour->fLocalGoal)
            {
                nodeNeighbour->node_parent = nodeCurrent;
                nodeNeighbour->fLocalGoal = fPossiblyLowerGoal;
                nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, node_end);
            }
        }
    }

    return true;
}

void MainWindow::draw_connections()
{
    for (int i = 0; i < 16; i++)
    {

            for (auto n : nodes[i].nodes_neighbour)
            {
                QGraphicsLineItem* lineItem = new QGraphicsLineItem();
                QPen pen((QColor("dimgray")));
                pen.setWidth(2);
                pen.setBrush(QBrush(QColor("dimgray")));
                lineItem->setPen(pen);
                lineItem->setLine(nodes[i].x+5, nodes[i].y-65,
                                  n->x+5, n->y-65);
                scene->addItem(lineItem);
            }
    }

}

void MainWindow::draw_nodes()
{


    for (int i = 0; i < 16; i++)
    {
            QGraphicsTextItem *text_node_id = scene->addText(QString::number(i));
            text_node_id->setPos(nodes[i].x-7, nodes[i].y-62);
            QGraphicsRectItem* rItem = new QGraphicsRectItem();
            rItem->setPos(nodes[i].x, nodes[i].y-70 );
            rItem->setRect(0,0, 10, 10);
            rItem->setBrush(QBrush(QColor(nodes[i].is_obstacle ? Qt::white : Qt::blue)));
            rItem->setPen(QPen(QColor(nodes[i].is_obstacle ? Qt::white : Qt::blue)));
            scene->addItem(rItem);

            if (nodes[i].is_visited)
            {
                QGraphicsRectItem* vItem = new QGraphicsRectItem();
                vItem->setPos(nodes[i].x, nodes[i].y-70 );
                vItem->setRect(0,0, 10, 10);
                vItem->setBrush(QBrush(QColor(Qt::darkBlue)));
                vItem->setPen(QPen(QColor(Qt::darkBlue)));
                scene->addItem(vItem);
            }
            if(&nodes[i] == node_start)
            {
                QGraphicsRectItem* sItem = new QGraphicsRectItem();
                sItem->setPos(nodes[i].x, nodes[i].y-70);
                sItem->setRect(0, 0, 10, 10);
                sItem->setBrush(QBrush(QColor(Qt::darkGreen)));
                sItem->setPen(QPen(QColor(Qt::darkGreen)));
                scene->addItem(sItem);
            }
            if(&nodes[i] == node_end)
            {
                QGraphicsRectItem* eItem = new QGraphicsRectItem();
                eItem->setPos(nodes[i].x, nodes[i].y-70);
                eItem->setRect(0,0, 10, 10);
                eItem->setBrush(QBrush(QColor(Qt::darkRed)));
                eItem->setPen(QPen(QColor(Qt::darkRed)));
                scene->addItem(eItem);
            }



    }

}

void MainWindow::draw_path()
{
    QVector<QString> temp_path;
    if (node_end != nullptr)
    {
        Node *p = node_end;
        temp_path.push_back(QString::number(p->node_id));
        while (p->node_parent != nullptr)
        {
            QGraphicsLineItem* lineItem = new QGraphicsLineItem();
            QPen pen((QColor(Qt::yellow)));
            pen.setWidth(UNIT_SIZE.width());
            pen.setBrush(QBrush(QColor(Qt::yellow)));
            lineItem->setPen(pen);
            lineItem->setLine(p->x + 5, p->y - 65,
                p->node_parent->x + 5, p->node_parent->y - 65);
            temp_path.push_back(QString::number(p->node_parent->node_id));
            scene->addItem(lineItem);

            // Set next node to this node's node_parent
            p = p->node_parent;
        }
    }
    std::reverse(temp_path.begin(), temp_path.end());
    path_run = temp_path;
    qDebug() << path_run;
    temp_path.clear();
}

void MainWindow::loop()
{
    ui->battery_progressBar->setValue(robot_battery);
    ui->water_c_progressBar->setValue(robot_water_capacity);


    if(node_end == nullptr)
    {
        // do nothing
    }
    else if(robot_current_node == node_end->node_id && flag_run_back == 0)
    {
        flag_run_back = 1;
        robot_mode = 1;
        node_start = node_end;
        node_end = &nodes[15];
        path_run.clear();
        solve_AStar();
        scene->clear();
        draw_connections();
        draw_nodes();
        draw_path();
        create_path_run(path_run_back);
        emit(send_path_run_back_signal(path_run_back));

        // send path run back (UART)
//        emit
    }

    else if(robot_current_node == node_end->node_id && flag_run_back == 1)
    {
        flag_run_back = 0;
        robot_mode = 0; // STOP/CHARGING
        node_start = &nodes[15];
        node_end = nullptr;
        path_run_back.clear();
        scene->clear();
        draw_connections();
        draw_nodes();
    }
}

//void MainWindow::keyPressEvent(QKeyEvent *event)
//{
//    if(!event->isAutoRepeat())
//    {
//        if(event->key() == Qt::Key_Z)
//        {
//            renderMainWindow();
//        }
//    }

//    if(event->key() == Qt::Key_Shift)
//    {
//        m_shiftPressed = true;
//    }
//    else if(event->key() == Qt::Key_Control)
//    {
//        m_controlPressed = true;
//    }

//    QGraphicsMainWindow::keyPressEvent(event);
//}

//void MainWindow::keyReleaseEvent(QKeyEvent *event)
//{
//    if(event->key() == Qt::Key_Shift)
//    {
//        m_shiftPressed = false;
//    }
//    else if(event->key() == Qt::Key_Control)
//    {
//        m_controlPressed = false;
//    }
//    QGraphicsMainWindow::keyReleaseEvent(event);
//}

//void MainWindow::mousePressEvent(QGraphicsMainWindowMouseEvent *event)
//{
//    run_btn_clicked = false;
//    QGraphicsMainWindow::mousePressEvent(event);
//}

//void MainWindow::mouseReleaseEvent(QGraphicsMainWindowMouseEvent *event)
//{
//    m_mousePosX = event->MainWindowPos().x();
//    m_mousePosY = event->MainWindowPos().y();
//    run_btn_clicked = true;

//    QGraphicsMainWindow::mouseReleaseEvent(event);
//}


void MainWindow::receive_is_connected_from_main(int value)
{
    qDebug() << "Receive from main:" << value;
    receive_value_connected = value;
    if(value){
        ui->connected_label->setText(QString(sc_window->portName +" is connected"));
        ui->connected_label->setStyleSheet("QLabel { color : Green;}");
        disconnect(sc_window, SIGNAL(pass_data_to_main(QString)), this, SLOT(receive_data_from_sc(QString)));
        connect(sc_window, SIGNAL(pass_data_to_main(QString)), this, SLOT(receive_data_from_sc(QString)));
    }
    else
        ui->connected_label->setText("Please set COM port!");
}

void MainWindow::receive_data_from_sc(QList<std::byte> data)
{
    qDebug() << "Receive data:" << data;
    // robot info
    if(data.at(0) == (std::byte)0xA0)
    {
        robot_speed = (uint8_t) data.at(1);
        robot_battery = (uint8_t) data.at(2);
        robot_water_capacity = (uint8_t) data.at(3);
    }
    // auto info
    else if(data.at(0) == (std::byte)0xA1)
    {
        robot_current_node = (uint8_t) data.at(1);
        robot_current_orient = (uint8_t) data.at(2);
        obstacle_distance = (uint8_t) data.at(3);

    }
}

void MainWindow::show_back(){
    ui->control_panel_groupBox->show();
    ui->general_status_groupBox->show();
}

void MainWindow::on_setting_pushButton_clicked()
{
    qDebug() << "clicked";
    if(sc_is_created == 0)
    {
        sc_window = new setting_com_window(this);
        qDebug() << "called";
        sc_is_created = 1;
    }
    sc_window->sc_is_connected = receive_value_connected;
    sc_window->show();
    disconnect(sc_window, SIGNAL(sc_send_is_connected(int)), this, SLOT(receive_is_connected_from_main(int)));
    connect(sc_window, SIGNAL(sc_send_is_connected(int)), this, SLOT(receive_is_connected_from_main(int)));
    connect(this, SIGNAL(send_path_run_signal(QList<QString>)), sc_window, SLOT(send_path_run(QList<QString>)));
    connect(this, SIGNAL(send_path_run_back_signal(QList<QString>)), sc_window, SLOT(send_path_run_back(QList<QString>)));
    connect(this, SIGNAL(send_mode_signal(int)), sc_window, SLOT(send_mode(int)));
    connect(this, SIGNAL(send_direction_signal(int)), sc_window, SLOT(send_direction(int)));
    connect(this, SIGNAL(send_start_task_node_signal(int)), sc_window, SLOT(send_start_task_node(int)));
}

void MainWindow::on_run_button_clicked()
{
    if(robot_mode == 0){ // STOP mode

        if(ui->velocity_textedit->toPlainText().toInt() == 0)
        {
            QMessageBox warning_set_velocity;
            warning_set_velocity.setText("WARNING!\nPlease set initial velocity!");
            warning_set_velocity.setIcon(QMessageBox::Warning);
            warning_set_velocity.setWindowTitle("Caution");
            warning_set_velocity.exec();

        }
        else{
            node_end = &nodes[ui->nodes_comboBox->currentText().toInt()];
            run_btn_clicked = true;
            solve_AStar();
            update_map();
            create_path_run(path_run);
            emit(send_path_run_signal(path_run));
            ui->graphicsView->setScene(scene);

            ui->pause_resume_button->setEnabled(true);
            robot_mode = 1; // auto mode
            // Change button to STOP
            QPixmap pixmap_stop(":/imgs/icon_image/stop.png");
            QIcon icon_stop(pixmap_stop);
            ui->run_button->setIcon(icon_stop);
            ui->run_button->setText("Stop");
        }
    }
    else{
        // Change button to RUN
        QPixmap pixmap_run(":/imgs/icon_image/icon_run.png");
        QIcon icon_run(pixmap_run);
        ui->run_button->setIcon(icon_run);
        ui->run_button->setText("Run");

        ui->pause_resume_button->setEnabled(false);
        robot_mode = 0; // STOP mode
    }

}


void MainWindow::on_pause_resume_button_clicked()
{

}


void MainWindow::on_nodes_i4_pushButton_clicked()
{
    node_i4_window->show();
    qDebug() << "clicked";
    ui->control_panel_groupBox->hide();
    ui->general_status_groupBox->hide();
    disconnect(node_i4_window, SIGNAL(hide_nodei4_window()), this, SLOT(show_back()));
    connect(node_i4_window, SIGNAL(hide_nodei4_window()), this, SLOT(show_back()));
}

