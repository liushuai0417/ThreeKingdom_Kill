#include "roomitem.h"
#include "ui_roomitem.h"

RoomItem::RoomItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoomItem)
{
    ui->setupUi(this);
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
