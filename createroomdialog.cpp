#include "createroomdialog.h"
#include "ui_createroomdialog.h"
#include<QPainter>
#include"mypushbutton.h"
CreateRoomDialog::CreateRoomDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateRoomDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/res/icon/icon.png"));
    this->setWindowTitle("三国Kill");
    ui->widget->setStyleSheet("QWidget:pane {border-top:0px solid #e8f3f9;background:  transparent; }"
                                "QTabBar::tab::selected{background: transparent;color:white;border:2px groove gray;border-radius:10px;padding:2px 4px;border-style:outset;font:12pt '华文行楷'}"
                                "QTabBar::tab{background: white;color:black;border-style:outset;border:2px groove gray;border-radius:10px;padding:2px 4px;font:12pt '华文行楷'}");
    ui->le_roomname->setStyleSheet("QLineEdit{background-color: rgba(96, 96, 96, 0.4)}"
                                         "QLineEdit{border-width:2;border-style:outset;border : 1px solid gray;}");
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::white);
    QBrush myBrush;
    QPalette palette;
    myBrush = QBrush(Qt::white,Qt::DiagCrossPattern);
    palette.setBrush( QPalette::Text,  myBrush);
    ui->le_roomname->setPalette(palette);
    ui->le_roomname->setFont(QFont("华文行楷",20,QFont::Bold));

    //创建房间按钮
    MyPushButton *createroom = new MyPushButton(":/res/icon/gu_click_createroom.png",":/res/icon/gu_click_createroom_1.png");
    createroom->setParent(this);
    createroom->move(this->width()*0.5-createroom->width()*0.5,this->height()*0.8-80);

    //创建房间按钮点击事件
    connect(createroom,&MyPushButton::clicked,this,&CreateRoomDialog::SLOT_CreatRoom);
}

CreateRoomDialog::~CreateRoomDialog()
{
    delete ui;
}

void CreateRoomDialog::paintEvent(QPaintEvent *event){
    //设置背景
    QPainter painter(thiss;
    QPixmap pix;
    pix.load(":/res/BJ/task_level_map.png");
    //pix = pix.scaled(pix.width()*2,pix.height()*2);
    painter.drawPixmap(0,0,this->width(),this->height(),pix);
}

//创建房间槽函数
void CreateRoomDialog::SLOT_CreatRoom(){
    QString roomname = ui->le_roomname->text();
    Q_EMIT SIG_CreateRoomCommit(roomname);
}
