#include "frienditem.h"
#include "ui_frienditem.h"

FriendItem::FriendItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FriendItem)
{
    ui->setupUi(this);
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::white);
    QBrush myBrush;
    QPalette palette;
    myBrush = QBrush(Qt::white,Qt::DiagCrossPattern);
    palette.setBrush( QPalette::Text,myBrush);
    ui->lb_feeling->setPalette(pe);
    ui->lb_name->setPalette(pe);
    ui->lb_state->setPalette(pe);

}

FriendItem::~FriendItem()
{
    delete ui;
}

//设置用户信息
void FriendItem::setItem(int iconid,QString name,QString feeling,int state){
    ui->lb_name->setText(name);
    ui->lb_feeling->setText(feeling);
    QPixmap pix;
    QString strPath;
    if(iconid<10){
        strPath = QString(":/res/TX/0%1.png").arg(iconid+1);
    }else{
        strPath = QString(":/res/TX/%1.png").arg(iconid+1);
    }
    pix.load(strPath);
    ui->pb_icon->setFixedSize(pix.width(),pix.height());
    ui->pb_icon->setStyleSheet("QPushButton{border:0px;}");//边框设置为0像素
    ui->pb_icon->setIcon(pix);
    ui->pb_icon->setIconSize(QSize(pix.width(),pix.height()));
    if(state == 0){
        ui->lb_state->setText("离线");
    }else{
        ui->lb_state->setText("在线");
    }
}

void FriendItem::paintEvent(QPaintEvent *event){
    ui->widget_2->setStyleSheet("QWidget{background-color: rgba(96, 96, 96, 0.2);}"
                                "QWidget{border-width:0;border-style:outset;}");
    ui->lb_feeling->setStyleSheet("QLabel {background-color: transparent;}");
    ui->lb_name->setStyleSheet("QLabel {background-color: transparent;}");
    ui->lb_state->setStyleSheet("QLabel {background-color: transparent;}");
}
