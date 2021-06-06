#include "chaneginfodialog.h"
#include "ui_chaneginfodialog.h"
#include"mypushbutton.h"
#include"QMessageBox"
//修改信息界面类
ChanegInfoDialog::ChanegInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChanegInfoDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/res/icon/icon.png"));
    this->setWindowTitle("三国Kill");
    ui->cb_icon->setIconSize(QSize(88,88));
    //设置头像下拉菜单
    for(int i=1;i<=10;i++){
        QString strPath = QString(":/res/TX/0%1.png").arg(i);
        ui->cb_icon->addItem(QIcon(strPath),"");
    }
    for(int i=11;i<=36;i++){
        QString strPath = QString(":/res/TX/%1.png").arg(i);
        ui->cb_icon->addItem(QIcon(strPath),"");
    }

    //修改信息按钮
    MyPushButton *alterinfo = new MyPushButton(":/res/icon/gu_click_changeinfo.png",":/res/icon/gu_click_changeinfo_1.png");
    alterinfo->setParent(this);
    alterinfo->move(this->width()*0.5-alterinfo->width()*0.5,this->height()*0.8-30);

    //修改信息按钮点击事件
    connect(alterinfo,&MyPushButton::clicked,this,&ChanegInfoDialog::SLOT_AlterInfo);
}

ChanegInfoDialog::~ChanegInfoDialog()
{
    delete ui;
}

void ChanegInfoDialog::paintEvent(QPaintEvent *event){

}

//修改信息槽函数
void ChanegInfoDialog::SLOT_AlterInfo(){
    //获取头像索引
    int index = ui->cb_icon->currentIndex();
    //昵称
    QString name = ui->le_name->text();
    //个性签名
    QString feeling = ui->le_feeling->text();
    QString strDefault = "比较懒，什么也没写";
    QString tmpName = name;
    QString tmpFeeling = feeling;
    if(tmpName.remove(" ").isEmpty()){
        QMessageBox::about(this,"提示","不能是空名字");
        return;
    }

    if(name.length()>10){
        QMessageBox::about(this,"提示","名字长了");
        return;
    }

    if(tmpFeeling.remove(" ").isEmpty()){
        feeling = strDefault;
    }
    if(feeling.length()>50){
        QMessageBox::about(this,"提示","签名长了");
        return;
    }

    //向kernel发送信号 提交修改信息
}
