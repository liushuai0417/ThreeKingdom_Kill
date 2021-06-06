#include "addfrienddialog.h"
#include "ui_addfrienddialog.h"
#include<QPainter>
#include"mypushbutton.h"
AddFriendDialog::AddFriendDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFriendDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/res/icon/icon.png"));
    this->setWindowTitle("三国Kill");
    ui->widget->setStyleSheet("QWidget:pane {border-top:0px solid #e8f3f9;background:  transparent; }"
                                "QTabBar::tab::selected{background: transparent;color:white;border:2px groove gray;border-radius:10px;padding:2px 4px;border-style:outset;font:12pt '华文行楷'}"
                                "QTabBar::tab{background: white;color:black;border-style:outset;border:2px groove gray;border-radius:10px;padding:2px 4px;font:12pt '华文行楷'}");
    ui->le_info->setStyleSheet("QLineEdit{background-color: rgba(96, 96, 96, 0.4)}"
                                         "QLineEdit{border-width:2;border-style:outset;border : 1px solid gray;}");

    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::white);
    QBrush myBrush;
    QPalette palette;
    myBrush = QBrush(Qt::white,Qt::DiagCrossPattern);
    palette.setBrush( QPalette::Text,  myBrush);
    ui->le_info->setPalette(palette);
    ui->le_info->setFont(QFont("华文行楷",20,QFont::Bold));

    //查找好友按钮
    MyPushButton *searchByName = new MyPushButton(":/res/icon/gu_click_addfriend.png",":/res/icon/gu_click_addfriend_1.png");
    searchByName->setParent(this);
    searchByName->move(this->width()*0.5-searchByName->width()*0.5,this->height()*0.8-80);
    connect(searchByName,&MyPushButton::clicked,this,&AddFriendDialog::SLOT_AddFriendByName);
}

AddFriendDialog::~AddFriendDialog()
{
    delete ui;
}

void AddFriendDialog::paintEvent(QPaintEvent *event){
    //设置背景
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/BJ/task_level_map.png");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);
}


//通过房间名查找的槽函数
void AddFriendDialog::SLOT_AddFriendByName(){
    QString str = ui->le_info->text();
    Q_EMIT SIG_AddFriendByNameCommit(str);
}
