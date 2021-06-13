#include "roomitem.h"
#include "ui_roomitem.h"
#include<QDebug>
#include<QDateTime>
//房间列表插件类
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
    ui->lb_number->setPalette(pe);
}


RoomItem::~RoomItem()
{
    delete ui;
}

//双击事件
void RoomItem::mouseDoubleClickEvent(QMouseEvent *event){

}

void RoomItem::setItem(int Roomid,QString RoomName,QString CreatorName,int num){

    ui->lb_roomname->setText(RoomName);
    ui->lb_roomid->setText(QString::number(Roomid));
    ui->lb_roomcreator->setText(CreatorName);
    ui->lb_number->setText(QString::number(num));
}



void RoomItem::paintEvent(QPaintEvent *event){
    ui->widget_2->setStyleSheet("QWidget{background-color: rgba(96, 96, 96, 0.2);}"
                                "QWidget{border-width:0;border-style:outset;}");
}

Ui::RoomItem *RoomItem::getUi() const
{
    return ui;
}
