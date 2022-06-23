#include "ad9361main.h"
#include "ui_ad9361main.h"
#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
QPushButton* agreeBut;
QPushButton* disagreeBut;

ad9361main::ad9361main(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ad9361main)
{
    ui->setupUi(this);

}

ad9361main::~ad9361main()
{
    delete ui;
}



void ad9361main::on_pushButton_CONNECT_clicked()
{

    qDebug()<<"CONNECT";
    if(1){
         result = QMessageBox::information(this, "通知","链接成功");
    }
}

void ad9361main::on_lineEdit_TXBW_editingFinished()
{
    qDebug()<<"TXBW";
}

void ad9361main::on_lineEdit_RXBW_editingFinished()
{
    qDebug()<<"RXBW";
}

void ad9361main::on_lineEdit_TXLO_editingFinished()
{
    qDebug()<<"TXLO";
}

void ad9361main::on_lineEdit_RXLO_editingFinished()
{
    qDebug()<<"RXLO";
}
