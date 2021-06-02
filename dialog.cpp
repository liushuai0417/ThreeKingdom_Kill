#include "dialog.h"
#include "ui_dialog.h"
#include<QPainter>
#include<QFont>
#include<QPushButton>
#include<QDebug>
#include<QMessageBox>
#include<QTabBar>
#include<QRegExp>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    //注册清空按钮点击事件
    connect(ui->pb_register_clear,&QPushButton::clicked,[=](){
        ui->le_email->clear();
        ui->le_password->clear();
        ui->le_password1->clear();
        ui->le_username->clear();
    });

    connect(ui->pb_login_clear,&QPushButton::clicked,[=](){
        ui->le_password_login->clear();
        ui->le_username_login->clear();
    });
    //注册按钮点击事件
    connect(ui->pb_register,&QPushButton::clicked,[=](){
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

        //返回登录结果

        //跳转到登录页
        ui->tabWidget->setCurrentIndex(1);
    });

    //登录按钮点击事件
    connect(ui->pb_login,&QPushButton::clicked,[=](){
        //获取字符串
        QString username = ui->le_username_login->text();
        QString password = ui->le_password_login->text();
        //验证输入账户密码是否为空
        if(username.isEmpty() || password.isEmpty()){
            QMessageBox::about(this,"提示","用户名或密码不能为空");
            return;
        }

        //转到游戏大厅

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
    ui->le_email->setStyleSheet("QLineEdit{background-color:transparent}"
                                         "QLineEdit{border-width:0;border-style:outset}");
    ui->le_password->setStyleSheet("QLineEdit{background-color:transparent}"
                                         "QLineEdit{border-width:0;border-style:outset}");
    ui->le_password1->setStyleSheet("QLineEdit{background-color:transparent}"
                                         "QLineEdit{border-width:0;border-style:outset}");
    ui->le_username->setStyleSheet("QLineEdit{background-color:transparent}"
                                         "QLineEdit{border-width:0;border-style:outset}");
    ui->le_password_login->setStyleSheet("QLineEdit{background-color:transparent}"
                                         "QLineEdit{border-width:0;border-style:outset}");
    ui->le_username_login->setStyleSheet("QLineEdit{background-color:transparent}"
                                         "QLineEdit{border-width:0;border-style:outset}");
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

    //设置登录按钮透明
    ui->pb_register->setStyleSheet( "QPushButton{background:rgba(0,0,0,0);color:white;font:25pt '华文行楷';border:1px solid rgba(0,0,0,0);}");
    ui->pb_login->setStyleSheet( "QPushButton{background:rgba(0,0,0,0);color:white;font:25pt '华文行楷';border:1px solid rgba(0,0,0,0);}");
    ui->pb_register_clear->setStyleSheet( "QPushButton{background:rgba(0,0,0,0);color:white;font:25pt '华文行楷';border:1px solid rgba(0,0,0,0);}");
    ui->pb_login_clear->setStyleSheet( "QPushButton{background:rgba(0,0,0,0);color:white;font:25pt '华文行楷';border:1px solid rgba(0,0,0,0);}");
}
