#include "showothercard.h"
#include "ui_showothercard.h"
#include<QPainter>
ShowOtherCard::ShowOtherCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowOtherCard)
{
    ui->setupUi(this);
    //配置场景
    //设置固定大小
    this->setFixedSize(800,300);
    //设置图标
    this->setWindowIcon(QIcon(":/res/icon/icon.png"));
    //设置窗口标题
    this->setWindowTitle("三国Kill");
}

ShowOtherCard::~ShowOtherCard()
{
    delete ui;
}

void ShowOtherCard::paintEvent(QPaintEvent *event){
    //设置背景
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/BJ/hallBg1.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);

    //设置背景上的图标
    //pix.load(":/res/icon/launcher_logo_big.png");
    //缩放
    //pix = pix.scaled(pix.width()*0.3,pix.height()*0.3);

    //painter.drawPixmap(10,30,pix.width(),pix.height(),pix);
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::white);
    QBrush myBrush;
    QPalette palette;
    myBrush = QBrush(Qt::white,Qt::DiagCrossPattern);
    palette.setBrush( QPalette::Text,myBrush);
}
