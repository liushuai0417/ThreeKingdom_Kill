#include "gaming.h"
#include "ui_gaming.h"
#include<QPainter>
Gaming::Gaming(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Gaming)
{
    ui->setupUi(this);
    //配置场景
    //设置固定大小
    this->setFixedSize(1900,900);
    //设置图标
    this->setWindowIcon(QIcon(":/res/icon/icon.png"));
    //设置窗口标题
    this->setWindowTitle("三国Kill");
}

void Gaming::paintEvent(QPaintEvent *event){
    //设置背景
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/BJ/background.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);
}

Gaming::~Gaming()
{
    delete ui;
}
