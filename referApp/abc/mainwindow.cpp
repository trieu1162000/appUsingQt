#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPainter line_path(this);
    QPen pen((QColor(Qt::yellow)));
    pen.setWidth(10);
    pen.setBrush(QBrush(QColor(Qt::yellow)));
////            lineItem->setPen(pen);
////            lineItem->setLine(p->x*node_size + node_size / 2, p->y*node_size + node_size / 2,
////                p->node_parent->x*node_size + node_size / 2, p->node_parent->y*node_size + node_size / 2);
////            scene->addItem(lineItem);
    line_path.setPen(pen);
//            line_path.drawLine(p->x, p->y, p->node_parent->x, p->node_parent->y);
//            // Set next node to this node's parent
//            p = p->node_parent;
//        }
//    }
line_path.drawLine(0, 0, 100, 100);
}

MainWindow::~MainWindow()
{
    delete ui;
}

