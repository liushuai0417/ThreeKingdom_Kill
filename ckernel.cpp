#include "ckernel.h"
#include"packdef.h"
#include<QDebug>
CKernel::CKernel(QObject *parent) : QObject(parent)
{

    m_Dialog = new Dialog;
    connect(m_Dialog,&Dialog::SIG_CLOSE,[=](){
        DestoryInstance();
    });
    m_Dialog->setWindowIcon(QIcon(":/res/icon/icon.png"));
    m_Dialog->setWindowTitle("三国Kill");
    m_Dialog->show();
    //函数指针
    void (Dialog::*RegisterSignal)(QString,QString,QString) = &Dialog::SIG_RegisterCommit;
    //注册槽函数
    m_tcpClient = new QMyTcpClient;
    m_tcpClient->setIpAndPort(DEF_SERVER_IP,8000);
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
}

void CKernel::DestoryInstance(){
    if(m_Dialog){
        m_Dialog->hide();
        delete m_Dialog;
        m_Dialog = NULL;
    }
}

