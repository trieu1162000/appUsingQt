#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->title_label->setStyleSheet("QLabel { font-weight : bold; font-size : 14pt; background-color : Lavender; color = Navy; }");
    ui->control_panel_groupBox->setStyleSheet("QGroupBox {background-color : Lavender; }");
    ui->general_status_groupBox->setStyleSheet("QGroupBox {background-color : Lavender; }");
    ui->control_panel_label->setStyleSheet("QLabel {font-weight : bold; color : DarkRed; }");
    ui->g_status_label->setStyleSheet("QLabel {font-weight : bold; color : DarkRed; }");

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_setting_pushButton_clicked()
{
    sc_window = new setting_com_window(this);
    sc_window->show();

}

