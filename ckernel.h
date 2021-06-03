#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>
#include"mainscene.h"
#include"dialog.h"
#include"qmytcpclient.h"
#include"packdef.h"
class CKernel : public QObject
{
    Q_OBJECT
public:
    //单例模式
    static CKernel* GetInstance(){
        static CKernel pKernel;
        return &pKernel;
    }
    void DestoryInstance();
private:
    explicit CKernel(QObject *parent = 0);
    ~CKernel(){}
    CKernel(const CKernel &kernel){}
    //void setNetPackMap();//设置协议映射表
signals:
private:
    Dialog *m_Dialog;//登录窗口指针
    MainScene *m_MainScene;//大厅指针
    QMyTcpClient *m_tcpClient;//网络指针
public slots:
};

#endif // CKERNEL_H
