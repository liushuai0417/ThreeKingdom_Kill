#include "friendlist.h"
#include "ui_friendlist.h"
#include<QPainter>
#include"mypushbutton.h"
FriendList::FriendList(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FriendList)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/res/icon/icon.png"));
    this->setWindowTitle("三国Kill");
    m_friendLayout = new QVBoxLayout;
    m_friendLayout->setContentsMargins(0,0,0,0);//设置外边距
    m_friendLayout->setSpacing(2);
    ui->wdg_frienditem->setLayout(m_friendLayout);

    //刷新列表按钮
    MyPushButton *regetFriendList = new MyPushButton(":/res/icon/click_regetfriendlist.png",":/res/icon/click_regetfriendlist_1.png");
    regetFriendList->setParent(this);
    regetFriendList->move(this->width()*0.5-regetFriendList->width()*0.5,this->height()*0.8-80);
}

FriendList::~FriendList()
{
    delete ui;
}


void FriendList::Slot_AddFriendItem(QWidget *item){
    m_friendLayout->addWidget(item);
}

void FriendList::Slot_RemoveFriendItem(QWidget *item){
    m_friendLayout->removeWidget(item);
}

void FriendList::paintEvent(QPaintEvent *event){
    //设置背景
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/BJ/task_level_map.png");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);
    ui->scrollArea->setStyleSheet("background-color:transparent;");
    ui->scrollArea->viewport()->setStyleSheet("background:transparent;");
    ui->wdg_frienditem->setStyleSheet("QWidget#wdg_frienditem{ border:1px solid white;background:  transparent; }");

    ui->widget->setStyleSheet("QWidget:pane {border-top:0px solid #e8f3f9;background:  transparent; }"
                                "QTabBar::tab::selected{background: transparent;color:white;border:2px groove gray;border-radius:10px;padding:2px 4px;border-style:outset;font:12pt '华文行楷'}"
                                "QTabBar::tab{background: white;color:black;border-style:outset;border:2px groove gray;border-radius:10px;padding:2px 4px;font:12pt '华文行楷'}");
}
