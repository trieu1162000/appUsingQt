#include "nodes_info.h"
#include "ui_nodes_info.h"

nodes_info::nodes_info(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::nodes_info)
{
//    rows =  ui->nodes_tableWidget->rowCount();
//    columns = ui->nodes_tableWidget->columnCount();
    ui->setupUi(this);

}

nodes_info::~nodes_info()
{
    delete ui;
}

int nodes_info::get_rows(){
        return ui->nodes_tableWidget->rowCount();
    //    columns = ui->nodes_tableWidget->columnCount();
}

int nodes_info::get_columns(){
    //    rows =  ui->nodes_tableWidget->rowCount();
        return ui->nodes_tableWidget->columnCount();
}

QStringList nodes_info::get_node_ids(){
    for(int i = 0; i < get_rows(); i++){
        QString node_id = ui->nodes_tableWidget->item(i, 0)->text();
        node_ids.append(node_id);
    }
    return node_ids;
}

int nodes_info::get_x(int node_id){
    return ui->nodes_tableWidget->item(node_id, 1)->text().toInt();
}

int nodes_info::get_y(int node_id){
    return ui->nodes_tableWidget->item(node_id, 2)->text().toInt();
}

QString nodes_info::get_neighbour(int node_id)
{
    return ui->nodes_tableWidget->item(node_id, 3)->text();
}



void nodes_info::on_pushButton_clicked()
{
    qDebug() << get_neighbour(3);
//    qDebug() << get_node_id(2);
    this->hide();
    emit(hide_nodei4_window());
}

