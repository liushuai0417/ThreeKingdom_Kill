#include "mainscene.h"
#include "ui_mainscene.h"
#include<QPainter>
#include<QLabel>
#include"mypushbutton.h"
#include<QDebug>
MainScene::MainScene(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainScene)
{
    ui->setupUi(this);

    //配置场景
    //设置固定大小
    this->setFixedSize(1500,900);
    //设置图标
    this->setWindowIcon(QIcon(":/res/icon/icon.png"));
    //设置窗口标题
    this->setWindowTitle("三国Kill");

    //创建房间按钮
    MyPushButton *createroom = new MyPushButton(":/res/icon/gu_click_createroom.png",":/res/icon/gu_click_createroom_1.png");
    createroom->setParent(this);
    createroom->move(180,this->height()*0.8-30);

    int index = createroom->width();//偏移量
    //加入房间按钮
    MyPushButton *joinroom = new MyPushButton(":/res/icon/gu_click_joinroom.png",":/res/icon/gu_click_joinroom_1.png");
    joinroom->setParent(this);
    joinroom->move(180+index+100,this->height()*0.8-30);

    //开始游戏按钮
    MyPushButton *startgame = new MyPushButton(":/res/icon/startbtn1.png",":/res/icon/startbtn2.png");
    startgame->setParent(this);
    startgame->move(this->width()*0.5-startgame->width()*0.5,this->height()*0.8-10);

    int index0 = this->width()*0.5-startgame->width()*0.5;
    int index1 = index0-180-index-100-index;

    //添加好友按钮
    MyPushButton *addfriend = new MyPushButton(":/res/icon/gu_click_addfriend.png",":/res/icon/gu_click_addfriend_1.png");
    addfriend->setParent(this);
    addfriend->move(index0+startgame->width()+index1,this->height()*0.8-30);

    //修改信息按钮
    int index2 = index+100;
    MyPushButton *alterinfo = new MyPushButton(":/res/icon/gu_click_changeinfo.png",":/res/icon/gu_click_changeinfo_1.png");
    alterinfo->setParent(this);
    alterinfo->move(index0+startgame->width()+index1+index2,this->height()*0.8-30);

    //五个按钮的信号和槽
    connect(createroom,&MyPushButton::clicked,this,&MainScene::Slot_CreateRoom);
    connect(joinroom,&MyPushButton::clicked,this,&MainScene::Slot_JoinRoom);
    connect(startgame,&MyPushButton::clicked,this,&MainScene::Slot_StartGame);
    connect(addfriend,&MyPushButton::clicked,this,&MainScene::Slot_AddFriend);
    connect(alterinfo,&MyPushButton::clicked,this,&MainScene::Slot_AlterInfo);

}

MainScene::~MainScene()
{
    delete ui;
}

void MainScene::paintEvent(QPaintEvent *event){
    //设置背景
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/BJ/hallBg1.jpg");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);

    //设置背景上的图标
    pix.load(":/res/icon/launcher_logo_big.png");
    //缩放
    pix = pix.scaled(pix.width()*0.3,pix.height()*0.3);
    painter.drawPixmap(10,30,pix.width(),pix.height(),pix);
}

//创建房间槽函数
void MainScene::Slot_CreateRoom(){
    //做弹起特效
    qDebug()<<__func__;
}

//添加好友槽函数
void MainScene::Slot_AddFriend(){
    qDebug()<<__func__;
}

//加入房间槽函数
void MainScene::Slot_JoinRoom(){
    qDebug()<<__func__;
}

//修改信息槽函数
void MainScene::Slot_AlterInfo(){
    qDebug()<<__func__;
}

//开始游戏槽函数
void MainScene::Slot_StartGame(){
    qDebug()<<__func__;
}
