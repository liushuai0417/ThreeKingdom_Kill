#include "ckernel.h"
#include"packdef.h"
#include<QDebug>
#include<QMessageBox>
#include<QLatin1String>
#include<QThread>
#include "ui_dialog.h"
#include"roomitem.h"
#define NetPackMap(a) m_NetPackMap[(a)-DEF_PACK_BASE]
CKernel::CKernel(QObject *parent) : QObject(parent)
{
    //初始化指针
    m_tcpClient = new QMyTcpClient;//网络指针
    m_Dialog = new Dialog;//登录注册指针
    m_Dialog->show();
    m_MainScene = new MainScene;//大厅指针

    //窗口关闭槽函数
    connect(m_Dialog,&Dialog::SIG_CLOSE,[=](){
        DestoryInstance();
    });
    setNetPackMap();//初始化映射数组
    //初始化IP 端口号
    m_tcpClient->setIpAndPort(DEF_SERVER_IP,8000);
    //函数指针
    void (Dialog::*RegisterSignal)(QString,QString,QString) = &Dialog::SIG_RegisterCommit;//注册信号
    void (Dialog::*LoginSignal)(QString,QString) = &Dialog::SIG_LoginCommit;//登录信号
    void (QMyTcpClient::*ReadyDataSignal)(char*,int) = &QMyTcpClient::SIG_ReadyData;//处理数据的信号
    //网络指针接收包内容的槽函数
    connect(m_tcpClient,ReadyDataSignal,this,&CKernel::SLOT_ReadyData);
    //注册槽函数
    connect(m_Dialog,RegisterSignal,[=](QString username,QString email,QString password){
        STRU_REGISTER_RQ rq;
        strcpy(rq.m_szUser,username.toStdString().c_str());
        strcpy(rq.m_szEmall,email.toStdString().c_str());
        strcpy(rq.m_szPassword,password.toStdString().c_str());
        qDebug()<<rq.m_szUser<<rq.m_szEmall<<rq.m_szPassword;
        //通过kernel类发送包
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
    });

    //登录槽函数
    connect(m_Dialog,LoginSignal,[=](QString username,QString password){
        STRU_LOGIN_RQ rq;
        strcpy(rq.m_szUser,username.toStdString().c_str());
        strcpy(rq.m_szPassword,password.toStdString().c_str());
        qDebug()<<rq.m_szUser<<rq.m_szPassword;
        //通过kernel发送包
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
    });

    connect(m_Dialog,&Dialog::SIG_AskRoomCommit,[=](){
        STRU_ASKROOM_RQ rq;
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
    });
    RoomItem *itemindex = new RoomItem;

    m_MainScene->Slot_AddUserItem(itemindex);
}

//接收数据槽函数
void CKernel::SLOT_ReadyData(char *buf,int nlen){
    //获取包类型
    int type = *(int*)buf;
    if(type>=DEF_PACK_BASE && type<=DEF_PACK_BASE+DEF_PACK_COUNT){
        PFUN fun = m_NetPackMap[type-DEF_PACK_BASE];
        if(fun){
            (this->*fun)(buf,nlen);
        }
        delete []buf;
    }
}

//销毁单例
void CKernel::DestoryInstance(){
    if(m_Dialog){
        m_Dialog->hide();
        delete m_Dialog;
        m_Dialog = NULL;
    }
    if(m_MainScene){
        m_MainScene->hide();
        delete m_MainScene;
        m_MainScene = NULL;
    }
    if(m_tcpClient){
        delete m_tcpClient;
        m_tcpClient = NULL;
    }
}

//设置协议映射表
void CKernel::setNetPackMap(){
    //清空数组
    memset(m_NetPackMap,0,sizeof(m_NetPackMap));
    NetPackMap(DEF_PACK_REGISTER_RS)  = &CKernel::SLOT_DealRegisterRs;
    NetPackMap(DEF_PACK_LOGIN_RS)  = &CKernel::SLOT_DealLoginRs;
    NetPackMap(DEF_PACK_ASKROOM_RS) = &CKernel::SLOT_DealAskRoomRs;
    qDebug()<<__func__<<DEF_PACK_LOGIN_RS-DEF_PACK_BASE;
}

//处理登录回复槽函数
void CKernel::SLOT_DealLoginRs(char *buf,int nlen){
    //拆包
    STRU_LOGIN_RS *rs = (STRU_LOGIN_RS *)buf;
    //判断请求包的结果
    switch(rs->m_lResult){
        case login_sucess:
            //初始化个人信息
            this->m_id = rs->m_userid;
            this->m_iconID = rs->m_userInfo.m_iconID;
            this->m_szName = QString(rs->m_userInfo.m_szName);
            this->m_feeling = QString(rs->m_userInfo.m_feeling);
            this->m_state = rs->m_userInfo.m_state;
            //登录成功
            QMessageBox::about(m_Dialog,"提示","登录成功");
            m_Dialog->hide();
            //m_MainScene = new MainScene;//大厅指针
            m_MainScene->setGeometry(m_Dialog->geometry());
            m_MainScene->show();
        break;
        default:
            QMessageBox::about(m_Dialog,"提示","用户名或密码错误");
        break;
    }
}

//处理注册回复槽函数
void CKernel::SLOT_DealRegisterRs(char *buf,int nlen){
    //拆包
    STRU_REGISTER_RS *rs = (STRU_REGISTER_RS *)buf;
    //判断请求包的结果
    switch(rs->m_lResult){
        //注册成功
        case register_sucess:
            QMessageBox::about(m_Dialog,"提示","注册成功");
            //跳转到登录页
            m_Dialog->getUi()->tabWidget->setCurrentIndex(1);
        break;
        case userid_is_exist:
            QMessageBox::about(m_Dialog,"提示","用户名已存在");
        break;
    }
}

void CKernel::SLOT_DealAskRoomRs(char *buf,int nlen){

    STRU_ASKROOM_RS *rs = (STRU_ASKROOM_RS *)buf;
    qDebug()<<rs->m_RoomList[0].sz_Roomname;
    QString roomname = QString(rs->m_RoomList[0].sz_Roomname);
    int roomid = rs->m_RoomList[0].m_Roomid;
    QString roomcreator = QString(rs->m_RoomList[0].sz_RoomCreator);
    RoomItem *item1 = new RoomItem;
    qDebug()<<"数组信息"<<roomname<<roomid<<roomcreator;
    item1->setItem(roomid,roomname,roomcreator);
    m_MainScene->Slot_AddUserItem(item1);
    //转到游戏大厅

}
