#include "roomitem.h"
#include "ui_roomitem.h"
#include<QDebug>
#include<QDateTime>
RoomItem::RoomItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoomItem)
{
    ui->setupUi(this);
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::white);
    QBrush myBrush;
    QPalette palette;
    myBrush = QBrush(Qt::white,Qt::DiagCrossPattern);
    palette.setBrush( QPalette::Text,myBrush);
    ui->lb_roomcreator->setPalette(pe);
    ui->lb_roomid->setPalette(pe);
    ui->lb_roomname->setPalette(pe);
}


RoomItem::~RoomItem()
{
    delete ui;
}

//双击事件
void RoomItem::mouseDoubleClickEvent(QMouseEvent *event){
    //进入房间


}

void RoomItem::setItem(int Roomid,QString RoomName,QString CreatorName){

    ui->lb_roomname->setText(RoomName);
    ui->lb_roomid->setText(QString::number(Roomid));
    ui->lb_roomcreator->setText(CreatorName);
}

void RoomItem::setTop(QString Roomid,QString RoomName,QString CreatorName){

}

void RoomItem::paintEvent(QPaintEvent *event){
    ui->widget_2->setStyleSheet("QWidget{background-color: rgba(96, 96, 96, 0.2)}"
                                "QWidget{border-width:0;border-style:outset;");
}
