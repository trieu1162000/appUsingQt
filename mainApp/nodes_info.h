#ifndef NODES_INFO_H
#define NODES_INFO_H

#include <QWidget>
#include <QStringList>

namespace Ui {
class nodes_info;
}

class nodes_info : public QWidget
{
    Q_OBJECT

public:
    explicit nodes_info(QWidget *parent = nullptr);
    ~nodes_info();
//    int rows;
//    int columns;
    int get_rows();
    int get_columns();
    int get_x(int node_id);
    int get_y(int node_id);
    QString get_neighbour(int node_id);
    QStringList node_ids;
    QStringList get_node_ids();

private slots:
    void on_pushButton_clicked();
signals:
    void hide_nodei4_window();
private:
    Ui::nodes_info *ui;
};

#endif // NODES_INFO_H
