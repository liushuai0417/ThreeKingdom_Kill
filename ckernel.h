#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>
#include"mainscene.h"
#include"dialog.h"
#include"qmytcpclient.h"
#include"packdef.h"
class CKernel;
typedef void (CKernel::*PFUN)(char*,int);
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
    void setNetPackMap();//设置协议映射表
    void MainSceneShow();
signals:

private:
    Dialog *m_Dialog;//登录窗口指针
    MainScene *m_MainScene;//大厅指针
    QMyTcpClient *m_tcpClient;//网络指针
    PFUN m_NetPackMap[DEF_PACK_COUNT];//协议映射数组
    int m_id;//用户id 唯一标识
    int m_iconID;//用户头像id
    int m_state;//用户状态
    QString m_szName;//用户昵称
    QString m_feeling;//个性签名
public slots:
    void SLOT_DealLoginRs(char *buf,int nlen);//处理登录回复槽函数
    void SLOT_DealRegisterRs(char *buf,int nlen);//处理注册回复槽函数
    void SLOT_ReadyData(char* buf,int nlen);
    void SLOT_DealAskRoomRs(char *buf,int nlen);//处理请求房间回复槽函数
};

#endif // CKERNEL_H
