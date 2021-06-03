#include "ckernel.h"

CKernel::CKernel(QObject *parent) : QObject(parent)
{
    m_Dialog = new Dialog;
    connect(m_Dialog,&Dialog::SIG_CLOSE,[=](){
        DestoryInstance();
    });
    m_Dialog->setWindowIcon(QIcon(":/res/icon/icon.png"));
    m_Dialog->setWindowTitle("三国Kill");
    //m_Dialog->show(m_Dialog,);
    //函数指针
    void (Dialog::*RegisterSignal)(QString,QString,QString) = &Dialog::SIG_RegisterCommit;
    connect(m_Dialog,RegisterSignal,[=](QString username,QString eamil,QString password){
        qDebug()<<username<<email<<password;
    });
}

void CKernel::DestoryInstance(){
    if(m_Dialog){
        m_Dialog->hide();
        delete m_Dialog;
        m_Dialog = NULL;
    }
}

