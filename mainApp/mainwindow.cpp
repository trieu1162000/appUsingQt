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
    connect(&m_timer, &QTimer::timeout, this, &MainWindow::update_loop);
    connect(&handle_task_timer, &QTimer::timeout, this, &MainWindow::handle_task_loop);
    connect(&handle_run_back_timer, &QTimer::timeout, this, &MainWindow::handle_run_back_loop);
//    m_elapsedTimer.start();
//    m_timer.start(m_loopTime);
//    handle_task_timer.start(m_loopTime);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    node_i4_window = new nodes_info(this);
    node_i4_window->hide();
    qDebug() << node_i4_window->get_rows();
    ui->nodes_comboBox->addItems(node_i4_window->get_node_ids());
    create_map();
    nodes->node_count = node_i4_window->get_rows();
    orient_matrix = create_orient_matrix();

    // Set rows and columns size
    ui->monitor_tableWidget->setColumnWidth(0, 135);
    ui->monitor_tableWidget->setColumnWidth(1, 135);
    ui->monitor_tableWidget->setColumnWidth(2, 135);
    ui->monitor_tableWidget->setColumnWidth(3, 135);

    ui->task_tableWidget->setColumnWidth(0, 30);
    ui->task_tableWidget->setColumnWidth(1, 270);
    ui->task_tableWidget->setColumnWidth(2, 50);
    ui->task_tableWidget->setColumnWidth(3, 95);
    ui->task_tableWidget->setColumnWidth(4, 95);
//    int myInt = 0xFF;
//    QByteArray bA;
//    qDebug() << bA;
//    QDataStream stream(&bA, QIODevice::WriteOnly);
//    stream << myInt;
//    qDebug() << bA;

}

MainWindow::~MainWindow()
{
    delete ui;
}


QVector<QString> MainWindow::create_path_run(QVector<QString> path)
{
    path.clear();
    for(int i = 0; i< path_run.size(); i++)
    {
        path.append(path_run[i]);
        if(i< path_run.size() -1)
            path.append(QString((orient_matrix[path_run[i].toInt()][path_run[i+1].toInt()])));
    }
    qDebug() << path;
    return path;
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
//    draw_path();
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

QVector<QString> MainWindow::draw_path(QVector<QString> temp_path)
{
    if (node_end != nullptr)
    {
        temp_path.clear();
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
    return temp_path;

}

void MainWindow::draw_task_path(QVector<QString> path)
{
    for(int i = 0; i < path.count()-1; i++)
    {
        Node *p = &nodes[path[i].toInt()];
        QGraphicsLineItem* lineItem = new QGraphicsLineItem();
        QPen pen((QColor(Qt::red)));
        pen.setWidth(UNIT_SIZE.width());
        pen.setBrush(QBrush(QColor(Qt::red)));
        lineItem->setPen(pen);
        lineItem->setLine(p->x + 5, p->y - 65,
            (p+1)->x + 5, (p+1)->y - 65);
        scene->addItem(lineItem);
    }

}


void MainWindow::handle_task_loop()
{
    if(task_to_do.isEmpty() || flag_run_back == 1 || robot_mode == 2)
    {
//        qDebug() << "called";
        handle_task_timer.stop();
        return;
    }
    if(task_to_do.begin()->is_completed == false && task_to_do.begin()->is_running == false)
    {
//        qDebug() << "called";
        scene->clear();
        draw_connections();
        draw_nodes();
        // create path run
        QString temp =  ui->task_tableWidget->item(task_to_do.begin()->task_id,1)->text();
        path_run_task = temp.split(" -> ");
        draw_task_path(path_run_task);
//        qDebug() << "path run task" << path_run_task;
        start_task_node = nodes[path_run_task.begin()->toInt()].node_id;
        node_end = &nodes[path_run_task.begin()->toInt()];
        for(int i = 1; i< path_run_task.count(); i++)
            nodes[path_run_task[i].toInt()].is_obstacle = true;
        path_run.clear();
        solve_AStar();
        path_run = draw_path(path_run);
        qDebug() << "path run" << path_run;
        path_run.removeLast();
        path_run = path_run + path_run_task;
        for(int i = 1; i< path_run_task.count(); i++)
            nodes[path_run_task[i].toInt()].is_obstacle = false;


//        qDebug() << path_run;
        // send path_run
        path_run = create_path_run(path_run);
        path_run.append("G");
        emit send_path_run_signal(path_run);
        qDebug() << "stop";
//        handle_task_timer.stop();

    }
    // if receive ACK
    // is_running = true
    if(task_to_do.begin()->is_running == true && task_to_do.begin()->is_completed == false)
    {
        QTableWidgetItem * item = new QTableWidgetItem();
        QTableWidgetItem * item2 = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignHCenter);
        item2->setTextAlignment(Qt::AlignHCenter);
        item->setText("Running");
        ui->task_tableWidget->setItem(task_to_do.begin()->task_id, 2, item);
        item2->setText(QTime::currentTime().toString("hh:mm:ss"));
        ui->task_tableWidget->setItem(task_to_do.begin()->task_id, 3, item2);
        handle_task_timer.stop();
        return;
    }

    // if receive is_complete
    if(task_to_do.begin()->is_completed == true)
    {
        QTableWidgetItem * item = new QTableWidgetItem();
        QTableWidgetItem * item2 = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignHCenter);
        item2->setTextAlignment(Qt::AlignHCenter);
        item->setText("Done");
        ui->task_tableWidget->setItem(task_to_do.begin()->task_id, 2, item);
        item2->setText(QTime::currentTime().toString("hh:mm:ss"));
        ui->task_tableWidget->setItem(task_to_do.begin()->task_id, 4, item2);
        flag_run_back = 1;
        task_to_do.removeFirst();
        handle_task_timer.stop();
        return;
    }


}

void MainWindow::handle_run_back_loop()
{
    if(flag_run_back == 0)
    {
        handle_run_back_timer.stop();
        return;
    }
    if(flag_run_back == 1 && robot_current_node == nodes[path_run_task.last().toInt()].node_id)
    {
        node_end = &nodes[15];
        node_start = &nodes[path_run_task.last().toInt()];
        path_run.clear();
        solve_AStar();
        scene->clear();
        draw_connections();
        draw_nodes();
        draw_path(path_run_back);
        // send path run back
        create_path_run(path_run_back);
        emit send_path_run_back_signal(path_run_back);
    }
    // if receive ACK
    // stop timer
    if(flag_run_back == 1 && robot_current_node == 15)
    {
        flag_run_back = 0;
        scene->clear();
        draw_connections();
        draw_nodes();
        return;
    }

}

void MainWindow::update_loop()
{
    ui->battery_progressBar->setValue(robot_battery);
    ui->water_c_progressBar->setValue(robot_water_capacity);
    velocity_item = new QTableWidgetItem;
    velocity_item->setTextAlignment(Qt::AlignCenter);
    current_node_item = new QTableWidgetItem;
    current_node_item->setTextAlignment(Qt::AlignCenter);
    current_orient_item = new QTableWidgetItem;
    current_orient_item->setTextAlignment(Qt::AlignCenter);
    status_item = new QTableWidgetItem;
    status_item->setTextAlignment(Qt::AlignCenter);
    switch (robot_mode){
        case 0:
            status_item->setText("Stop");
            ui->monitor_tableWidget->setItem(0, 0, status_item);
            break;
        case 1:
            status_item->setText("Running");
            ui->monitor_tableWidget->setItem(0, 0, status_item);
            break;
        default:
            status_item->setText("Stop");
            ui->monitor_tableWidget->setItem(0, 0, status_item);
            break;

    }

    velocity_item->setText(QString::number(robot_speed));
    ui->monitor_tableWidget->setItem(0, 1, velocity_item);
    current_orient_item->setText(QString((char)robot_current_orient));
    ui->monitor_tableWidget->setItem(0, 2, current_orient_item);
    current_node_item->setText(QString::number(robot_current_node));
    ui->monitor_tableWidget->setItem(0, 3, current_node_item);


    // if receive ACK path run
//            handle_task_timer.start(m_loopTime);
    // if receive task_complete && task_to_do.begin()->is_running = true
//        task_to_do.begin()->is_completed = true;
//          task_to_do.begin()->is_completed = false;
//            handle_task_timer.start(m_loopTime);

    // if receive ACK path_run_back
//            handle_run_back_timer.start(m_loopTime);
//        flag_run_back = 1;


    if(robot_current_node == 15 && flag_run_back == 0 && !handle_task_timer.isActive())
    {
        handle_task_timer.start(m_loopTime);

    }

    if(robot_current_node == path_run_task.last().toInt() && flag_run_back == 1 && !handle_run_back_timer.isActive())
    {
        handle_run_back_timer.start(m_loopTime);
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
//    receive_value_connected = value;
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
//    qDebug() << "clicked";
    if(sc_is_created == 0)
    {
        sc_window = new setting_com_window(this);
        sc_is_created = 1;
    }
//    sc_window->sc_is_connected = receive_value_connected;
    sc_window->show();
    disconnect(sc_window, SIGNAL(sc_send_is_connected(int)), this, SLOT(receive_is_connected_from_main(int)));
    connect(sc_window, SIGNAL(sc_send_is_connected(int)), this, SLOT(receive_is_connected_from_main(int)));
    connect(this, SIGNAL(send_path_run_signal(QList<QString>)), sc_window, SLOT(send_path_run(QList<QString>)));
    connect(this, SIGNAL(send_path_run_back_signal(QList<QString>)), sc_window, SLOT(send_path_run_back(QList<QString>)));
    connect(this, SIGNAL(send_mode_signal(int)), sc_window, SLOT(send_mode(int)));
    connect(this, SIGNAL(send_direction_signal(int)), sc_window, SLOT(send_direction(int)));
    connect(this, SIGNAL(send_start_task_node_signal(int)), sc_window, SLOT(send_start_task_node(int)));
//    qDebug() << "called";
}

void MainWindow::on_run_button_clicked()
{
    task_to_do.clear();
    for(int i = 0; i<ui->task_tableWidget->rowCount(); i++)
    {
        if(ui->task_tableWidget->item(i, 2)->text() == "Waiting")
        {
            task temp;
            temp.is_completed = false;
            temp.is_running = false;
            temp.task_id = i;
            task_to_do.append(temp);
        }
    }


    if(task_to_do.size() == 0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("No task available in list!");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        return;
    }
    if(run_btn_clicked == false){ // STOP mode

        // check if robot is available
        if(robot_current_node != 15)
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Warning");
            msgBox.setText("Robot is not available to run!");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
            return;
        }

        ui->pause_resume_button->setEnabled(true);
        run_btn_clicked = true; // auto mode

        // Change button to STOP
        QPixmap pixmap_stop(":/imgs/icon_image/stop.png");
        QIcon icon_stop(pixmap_stop);
        ui->run_button->setIcon(icon_stop);
        ui->run_button->setText("Stop");
        m_timer.start(m_loopTime);
        handle_task_timer.start(m_loopTime);
    }
    else{
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Do you want to Stop?");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon(QMessageBox::Question);

        int ret = msgBox.exec();
        QPixmap pixmap_run(":/imgs/icon_image/icon_run.png");
        QIcon icon_run(pixmap_run);
        switch (ret)
         {
          case QMessageBox::Yes:
                // Change button to RUN
                ui->run_button->setIcon(icon_run);
                ui->run_button->setText("Run");
                ui->pause_resume_button->setEnabled(false);
                robot_mode = 0; // STOP mode
                status_item = new QTableWidgetItem;
                status_item->setTextAlignment(Qt::AlignCenter);
                status_item->setText("Stop");
                ui->monitor_tableWidget->setItem(0, 0, status_item);

                flag_run_back = 1;
                break;
          default:
                break;
        }

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



bool MainWindow::check_path(QString cur_node, QString next_node)
{
    int count = 0;
    QString n_nodes = node_i4_window->get_neighbour(cur_node.toInt());
    QStringList n_nodes_list = n_nodes.split(", ");
    foreach(QString n_node, n_nodes_list){
        if(n_node == next_node)
            count ++;
    }
    if(count)
        return true;
    else
        return false;
}


void MainWindow::on_add_node_pushButton_clicked()
{
    QString node_add = QString(ui->nodes_comboBox->currentText());
    if(path_chose.isEmpty()){
        path_chose.append(node_add);
    }
    else
    {
        if(check_path(path_chose.last(), node_add))
            path_chose.append(node_add);
        else
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Warning");
            msgBox.setText("Can not add this node!");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();

        }
    }
    qDebug() << path_chose;
    QString temp = "";
    for(int i = 0; i<path_chose.count(); i++)
    {
        if(path_chose.at(i) == path_chose.last() && path_chose.count()>1)
            temp.append(QString(path_chose.at(i)));
        else
            temp.append(QString(path_chose.at(i)+ " -> "));
    }
    ui->path_label->setText(temp);
//    node_start =

}


void MainWindow::on_ok_node_pushButton_clicked()
{
    if(path_chose.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Path is empty. Please set path to add!");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        return;
    }
    QTableWidgetItem *item = nullptr;
    ui->task_tableWidget->setRowCount(ui->task_tableWidget->rowCount() + 1);

    for(int j = 0; j < ui->task_tableWidget->columnCount(); j++)
    {
        item = new QTableWidgetItem;
        if(j == 0)
            item->setText(QString::number(ui->task_tableWidget->rowCount()));
        if(j == 1)
            item->setText(ui->path_label->text());
        if(j == 2)
            item->setText("Waiting");
        ui->task_tableWidget->setItem(ui->task_tableWidget->rowCount() -1, j, item);
    }
    path_chose.clear();
    ui->path_label->setText("");
}


void MainWindow::on_clear_path_pushButton_clicked()
{
    path_chose.clear();
    ui->path_label->setText("");
}


void MainWindow::on_manual_mode_button_clicked()
{
    this->hide();

}

