#include "mainscene.h"
#include "ui_mainscene.h"
#include<QPainter>
#include<QLabel>
#include"mypushbutton.h"
#include<QDebug>
#include<QWidget>
#include<chaneginfodialog.h>
#include<QScrollArea>
#include<friendlist.h>
#include<frienditem.h>
//大厅界面
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

    //刷新房间列表按钮
    MyPushButton *reget = new MyPushButton(":/res/icon/click_reget_2.png",":/res/icon/click_reget_1.png");
    reget->setParent(this);
    reget->move(400,this->height()*0.8-400);

    //获取好友列表按钮
    MyPushButton *getFriendList = new MyPushButton(":/res/icon/getfriendlist.png",":/res/icon/getfriendlist_1.png");
    getFriendList->setParent(this);
    getFriendList->move(this->width()-200,this->height()*0.8-400);
    //六个按钮的信号和槽
    connect(createroom,&MyPushButton::clicked,this,&MainScene::Slot_CreateRoom);
    connect(joinroom,&MyPushButton::clicked,this,&MainScene::Slot_JoinRoom);
    connect(startgame,&MyPushButton::clicked,this,&MainScene::Slot_StartGame);
    connect(addfriend,&MyPushButton::clicked,this,&MainScene::Slot_AddFriend);
    connect(alterinfo,&MyPushButton::clicked,this,&MainScene::Slot_AlterInfo);
    connect(reget,&MyPushButton::clicked,this,&MainScene::Slot_RegetRoom);
    connect(getFriendList,&MyPushButton::clicked,this,&MainScene::Slot_GetFriendList);
    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setContentsMargins(0,0,0,0);//设置外边距
    m_mainLayout->setSpacing(2);
    ui->wdg_roomlist->setLayout(m_mainLayout);


}

void MainScene::Slot_AddUserItem(QWidget *item){
    m_mainLayout->addWidget(item);
}

void MainScene::Slot_RemoveUserItem(QWidget *item){
    m_mainLayout->removeWidget(item);
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

    ui->scrollArea->setStyleSheet("background-color:transparent;");
    ui->scrollArea->viewport()->setStyleSheet("background:transparent;");
    ui->wdg_info->setStyleSheet("QWidget#wdg_info{ border:1px solid white;background:  transparent; }");
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::white);
    QBrush myBrush;
    QPalette palette;
    myBrush = QBrush(Qt::white,Qt::DiagCrossPattern);
    palette.setBrush( QPalette::Text,myBrush);
    ui->lb_feeling->setPalette(pe);
    ui->lb_name->setPalette(pe);
}

//创建房间槽函数
void MainScene::Slot_CreateRoom(){
    Q_EMIT SIG_CreateRoom();
}

//添加好友槽函数
void MainScene::Slot_AddFriend(){
    Q_EMIT SIG_AddFriend();
}

//加入房间槽函数
void MainScene::Slot_JoinRoom(){
    Q_EMIT SIG_JoinRoom();
}

//修改信息槽函数
void MainScene::Slot_AlterInfo(){
    Q_EMIT SIG_ShowAlterInfo();
}

//开始游戏槽函数
void MainScene::Slot_StartGame(){

}

//好友列表槽函数
void MainScene::Slot_GetFriendList(){
    Q_EMIT SIG_GetFriendList();
}

//刷新房间列表槽函数
void MainScene::Slot_RegetRoom(){
    Q_EMIT SIG_ReGetRoomTable();
}

Ui::MainScene *MainScene::getUi() const
{
    return ui;
}
