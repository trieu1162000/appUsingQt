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

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::create_map()
{
    nodes = new Node[node_i4_window->get_rows()];
    for(int i = 0; i < 16; i++){
        nodes[i].node_parent = nullptr;
        nodes[i].is_visited = false;
        nodes[i].is_obstacle = false;
        nodes[i].x = node_i4_window->get_x(i);
        nodes[i].y = node_i4_window->get_y(i);
        QString n_nodes = node_i4_window->get_neighbour(i);
        QStringList n_nodes_list = n_nodes.split(", ");
        foreach(QString n_node, n_nodes_list){
            nodes[i].nodes_neighbour.push_back(&nodes[n_node.toInt()]);
            qDebug() << n_node.toInt();
        }

    }

    node_start = &nodes[15];
    ui->robot_label->setGeometry(nodes[15].x-15, nodes[15].y+45, 61, 31 );
}

void MainWindow::update_map()
{

    if(run_btn_clicked)
    {


        solve_AStar(); // Solve in "real-time" gives a nice effect
        run_btn_clicked = false;

    }
    //drawing
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
            text_node_id->setPos(nodes[i].x+5, nodes[i].y-62);
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
    if (node_end != nullptr)
    {
        Node *p = node_end;
        while (p->node_parent != nullptr)
        {
            QGraphicsLineItem* lineItem = new QGraphicsLineItem();
            QPen pen((QColor(Qt::yellow)));
            pen.setWidth(UNIT_SIZE.width());
            pen.setBrush(QBrush(QColor(Qt::yellow)));
            lineItem->setPen(pen);
            lineItem->setLine(p->x + 5, p->y - 65,
                p->node_parent->x + 5, p->node_parent->y - 65);
            scene->addItem(lineItem);

            // Set next node to this node's node_parent
            p = p->node_parent;
        }
    }
}



void MainWindow::loop()
{
    m_deltaTime = m_elapsedTimer.elapsed();
    m_elapsedTimer.restart();

    m_loopTime += m_deltaTime;
    if( m_loopTime > m_loopSpeed)
    {
        m_loopTime -= m_loopSpeed;
        update_map();
    }
    qDebug() << "Called";
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
        ui->connected_label->setText("COM4 is connected");
        ui->connected_label->setStyleSheet("QLabel { color : Green;}");
    }
    else
        ui->connected_label->setText("Please set COM port!");
}

void MainWindow::receive_data_from_sc(QString data)
{
    qDebug() << "Receive data:" << data;
    robot_info = data;
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
        sc_is_created = 1;
    }
    sc_window->sc_is_connected = receive_value_connected;
    sc_window->show();
    disconnect(sc_window, SIGNAL(sc_send_is_connected(int)), this, SLOT(receive_is_connected_from_main(int)));
    connect(sc_window, SIGNAL(sc_send_is_connected(int)), this, SLOT(receive_is_connected_from_main(int)));
}

void MainWindow::on_run_button_clicked()
{
    if(robot_status == 0){

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
            ui->graphicsView->setScene(scene);

            disconnect(sc_window, SIGNAL(pass_data_to_main(QString)), this, SLOT(receive_data_from_sc(QString)));
            connect(sc_window, SIGNAL(pass_data_to_main(QString)), this, SLOT(receive_data_from_sc(QString)));
            ui->pause_resume_button->setEnabled(true);
            robot_status = 1;
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
        robot_status = 0;
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

