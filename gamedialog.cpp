#include "gamedialog.h"
#include "ui_gamedialog.h"
#include<QPainter>
GameDialog::GameDialog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameDialog)
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

GameDialog::~GameDialog()
{
    delete ui;
}

void GameDialog::paintEvent(QPaintEvent *event){
    //设置背景
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/BJ/background.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);
}

void GameDialog::closeEvent(QCloseEvent *event){
    event->accept();
    Q_EMIT SIG_LeaveRoom(this->roomid);
}
