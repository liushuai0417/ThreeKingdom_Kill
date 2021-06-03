#include "ckernel.h"
#include"packdef.h"
#include<QDebug>
#include<QMessageBox>
#include<QLatin1String>
#include<QThread>
#define NetPackMap(a) m_NetPackMap[(a)-DEF_PACK_BASE]
CKernel::CKernel(QObject *parent) : QObject(parent)
{
    //初始化指针

    qDebug()<<__func__<<QThread::currentThreadId();
    m_tcpClient = new QMyTcpClient;//网络指针
    m_Dialog = new Dialog;//登录注册指针
    m_Dialog->setWindowIcon(QIcon(":/res/icon/icon.png"));
    m_Dialog->setWindowTitle("三国Kill");
    m_Dialog->show();
    m_MainScene = new MainScene;//大厅指针

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

    connect(m_tcpClient,ReadyDataSignal,[=](char* buf,int nlen){
        qDebug()<<"11111";
        int type = *(int*)buf;
        qDebug()<<type;
        if(type>=DEF_PACK_BASE && type<=DEF_PACK_BASE+DEF_PACK_COUNT){
            PFUN fun = m_NetPackMap[type-DEF_PACK_BASE];
            if(fun){
                (this->*fun)(buf,nlen);
            }
        }
        delete []buf;
    });

    //注册槽函数
    connect(m_Dialog,RegisterSignal,[=](QString username,QString email,QString password){
        qDebug()<<username<<email<<password;
        STRU_REGISTER_RQ rq;
        strcpy(rq.m_szUser,username.toStdString().c_str());
        strcpy(rq.m_szEmall,email.toStdString().c_str());
        strcpy(rq.m_szPassword,password.toStdString().c_str());
        qDebug()<<rq.m_szUser<<rq.m_szEmall<<rq.m_szPassword;
        //通过kernel类发送包
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
    });
    //MainSceneShow();
    //登录槽函数
    connect(m_Dialog,LoginSignal,[=](QString username,QString password){
        qDebug()<<username<<password;
        STRU_LOGIN_RQ rq;
        strcpy(rq.m_szUser,username.toStdString().c_str());
        strcpy(rq.m_szPassword,password.toStdString().c_str());
        qDebug()<<rq.m_szUser<<rq.m_szPassword;
        //通过kernel发送包
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
    });

//    connect(this,&CKernel::SIG_MainSceneShow,[=](){

//    });
}

void CKernel::DestoryInstance(){
    if(m_Dialog){
        m_Dialog->hide();
        delete m_Dialog;
        m_Dialog = NULL;
    }
//    if(m_MainScene){
//        m_MainScene->hide();
//        delete m_MainScene;
//        m_MainScene = NULL;
//    }
    if(m_tcpClient){
        delete m_tcpClient;
        m_tcpClient = NULL;
    }
}

void CKernel::MainSceneShow(){
    m_Dialog->hide();
    m_MainScene = new MainScene;//大厅指针
    //m_MainScene->setParent(m_Dialog);
    //m_MainScene->setGeometry(m_Dialog->geometry());
    m_MainScene->show();
}

//设置协议映射表
void CKernel::setNetPackMap(){

    //清空数组
    memset(m_NetPackMap,0,sizeof(m_NetPackMap));
    m_NetPackMap[DEF_PACK_REGISTER_RS-DEF_PACK_BASE] = &CKernel::SLOT_DealRegisterRs;
    m_NetPackMap[DEF_PACK_LOGIN_RS-DEF_PACK_BASE] = &CKernel::SLOT_DealLoginRs;
    qDebug()<<__func__<<DEF_PACK_LOGIN_RS-DEF_PACK_BASE;
}

//处理登录回复槽函数
void CKernel::SLOT_DealLoginRs(char *buf,int nlen){
    qDebug()<<__func__<<QThread::currentThreadId();
    //拆包
    STRU_LOGIN_RS *rs = (STRU_LOGIN_RS *)buf;
    //判断请求包的结果
    switch(rs->m_lResult){
        case login_sucess:
            //初始化个人信息
            this->m_id = rs->m_userid;
            this->m_iconID = rs->m_userInfo.m_iconID;
            this->m_szName = QString(QLatin1String(rs->m_userInfo.m_szName));
            this->m_feeling = QString(QLatin1String(rs->m_userInfo.m_feeling));
            this->m_state = rs->m_userInfo.m_state;
            //登录成功
            //QMessageBox::about(m_Dialog,"提示","登录成功");
            qDebug()<<__func__;
            qDebug()<<m_id<<m_iconID<<m_szName<<m_feeling<<m_state;
            //转到游戏大厅
            m_Dialog->~Dialog();
            m_MainScene ->show();//大厅指针

        break;
        default:
            //QMessageBox::about(m_Dialog,"提示","用户名或密码错误");
        break;
    }
}

//处理注册回复槽函数
void CKernel::SLOT_DealRegisterRs(char *buf,int nlen){
    //拆包

}
