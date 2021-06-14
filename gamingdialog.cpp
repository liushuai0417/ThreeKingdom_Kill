#include "gamingdialog.h"
#include "ui_gamingdialog.h"
#include<QPainter>
GamingDialog::GamingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GamingDialog)
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

GamingDialog::~GamingDialog()
{
    delete ui;
}

void GamingDialog::paintEvent(QPaintEvent *event){
    //设置背景
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/BJ/background.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);
}

void GamingDialog::closeEvent(QCloseEvent *event){
    event->accept();
    Q_EMIT SIG_LeaveRoom(this->roomid);
}
