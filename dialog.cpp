#include "dialog.h"
#include "ui_dialog.h"
#include<QPainter>
#include<QFont>
#include<QPushButton>
#include<QDebug>
#include<QMessageBox>
#include<QTabBar>
#include<QRegExp>
#include<QIcon>
#include<mypushbutton.h>
//登录窗口
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);


    //登录按钮
    MyPushButton *login = new MyPushButton(":/res/icon/loginbtn1.png",":/res/icon/loginbtn2.png");
    login->setParent(ui->tab_login);
    login->move(this->width()*0.5-login->width()*0.5,this->height()*0.8);

    //注册按钮
    MyPushButton *regis = new MyPushButton(":/res/icon/register_1.png",":/res/icon/register_2.png");
    regis->setParent(ui->tab_register);
    regis->move(this->width()*0.5-login->width()*0.5,this->height()*0.8);
    //注册按钮
    //登录按钮点击事件
    connect(login,&MyPushButton::clicked,[=](){
        //获取字符串
        QString username = ui->le_username_login->text();
        QString password = ui->le_password_login->text();
        //验证输入账户密码是否为空
        if(username.isEmpty() || password.isEmpty()){
            QMessageBox::about(this,"提示","用户名或密码不能为空");
            return;
        }
        qDebug()<<username<<password;
        Q_EMIT SIG_LoginCommit(username,password);

    });

    connect(regis,&MyPushButton::clicked,[=](){
        //获取lineedit的内容
        QString username = ui->le_username->text();
        QString email = ui->le_email->text();
        QString password = ui->le_password->text();
        QString password_config = ui->le_password1->text();
        if(username.isEmpty()||email.isEmpty()||password.isEmpty()){
            QMessageBox::about(this,"提示","用户名或密码或邮箱不能为空");
            return;
        }
        //正则表达式验证邮箱格式是否正确
        QRegExp exep("[a-zA-Z0-9_]\{1,\}\@[a-zA-Z0-9]\{1,\}\.\(net\|com\)");
        bool flag = exep.exactMatch(email);
        if(!flag){
            QMessageBox::about(this,"提示","邮箱格式非法,请重新输入");
            return;
        }
        if(password.length()>15){
            QMessageBox::about(this,"提示","密码过长");
            return;
        }
        if(password != password_config){
            QMessageBox::about(this,"提示","两次输入的密码不一致");
            return;
        }

        //向kernel发送信号
        Q_EMIT SIG_RegisterCommit(username,email,password);
        //返回登录结果

        //跳转到登录页
        //ui->tabWidget->setCurrentIndex(1);
    });
}

Dialog::~Dialog()
{
    delete ui;
}

//重写绘图事件
void Dialog::paintEvent(QPaintEvent *event){
    //设置wideget背景
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/BJ/loading_bg.png");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);

    //设置tabWidget背景透明
    ui->tabWidget->setStyleSheet("QTabWidget:pane {border-top:0px solid #e8f3f9;background:  transparent; }"
                                 "QTabBar::tab::selected{background: transparent;color:white;border:2px groove gray;border-radius:10px;padding:2px 4px;border-style:outset;font:12pt '华文行楷'}"
                                 "QTabBar::tab{background: white;color:black;border-style:outset;border:2px groove gray;border-radius:10px;padding:2px 4px;font:12pt '华文行楷'}");
    //设置lineedit透明 无边框
    ui->le_email->setStyleSheet("QLineEdit{background-color: rgba(96, 96, 96, 0.4)}"
                                "QLineEdit{border-width:2;border-style:outset;border : 2px solid white;}");
    ui->le_password->setStyleSheet("QLineEdit{background-color: rgba(96, 96, 96, 0.4)}"
                                   "QLineEdit{border-width:2;border-style:outset;border : 2px solid white;}");
    ui->le_password1->setStyleSheet("QLineEdit{background-color: rgba(96, 96, 96, 0.4)}"
                                    "QLineEdit{border-width:2;border-style:outset;border : 2px solid white;}");
    ui->le_username->setStyleSheet("QLineEdit{background-color: rgba(96, 96, 96, 0.4)}"
                                   "QLineEdit{border-width:2;border-style:outset;border : 2px solid white;}");
    ui->le_password_login->setStyleSheet("QLineEdit{background-color: rgba(96, 96, 96, 0.4)}"
                                         "QLineEdit{border-width:2;border-style:outset;border : 2px solid white;}");
    ui->le_username_login->setStyleSheet("QLineEdit{background-color: rgba(96, 96, 96, 0.4)}"
                                         "QLineEdit{border-width:2;border-style:outset;border : 2px solid white;}");
    //设置字体颜色
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::white);
    QBrush myBrush;
    QPalette palette;
    myBrush = QBrush(Qt::white,Qt::DiagCrossPattern);
    palette.setBrush( QPalette::Text,  myBrush);
    ui->le_email->setPalette(palette);
    ui->le_password->setPalette(palette);
    ui->le_password1->setPalette(palette);
    ui->le_username->setPalette(palette);
    ui->le_email->setFont(QFont("华文行楷",20,QFont::Bold));
    ui->le_password->setFont(QFont("华文行楷",15,QFont::Bold));
    ui->le_password1->setFont(QFont("华文行楷",15,QFont::Bold));
    ui->le_username->setFont(QFont("华文行楷",20,QFont::Bold));
    ui->lb_email->setPalette(pe);
    ui->lb_password->setPalette(pe);
    ui->lb_password1->setPalette(pe);
    ui->lb_username->setPalette(pe);

    ui->le_password_login->setPalette(palette);
    ui->le_username_login->setPalette(palette);
    ui->lb_password_login->setPalette(pe);
    ui->lb_username_login->setPalette(pe);
    ui->le_password_login->setFont(QFont("华文行楷",15,QFont::Bold));
    ui->le_username_login->setFont(QFont("华文行楷",20,QFont::Bold));


}
