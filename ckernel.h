#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>
#include"mainscene.h"
#include"dialog.h"
#include"qmytcpclient.h"
#include"packdef.h"
#include"chaneginfodialog.h"
#include"createroomdialog.h"
#include"addfrienddialog.h"
#include"joinroomdialog.h"
#include<vector>
#include"roomitem.h"
#include"friendlist.h"
#include"frienditem.h"
#include"gamingdialog.h"
#include"mypushbutton.h"
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
    void SIG_ShowIdentity(int identity);
private:
    Dialog *m_Dialog;//登录窗口指针
    MainScene *m_MainScene;//大厅指针
    QMyTcpClient *m_tcpClient;//网络指针
    ChanegInfoDialog *changeDialog;//修改个人信息窗口指针
    CreateRoomDialog *createDialog;//创建房间窗口指针
    AddFriendDialog *addDialog;//添加好友
    JoinRoomDialog *joinDialog;//加入房间
    FriendList* friendlistDialog;//好友列表窗口
    GamingDialog *gamingdlg;//游戏界面指针
    MyPushButton *identity;//身份牌
    PFUN m_NetPackMap[DEF_PACK_COUNT];//协议映射数组
    RoomItem *item;
    int m_id;//用户id 唯一标识
    int m_iconID;//用户头像id
    int m_state;//用户状态
    QString m_szName;//用户昵称
    QString m_feeling;//个性签名
    int m_friendid;//查询好友id
    bool m_roomcount;//限制room插件插入次数
    bool m_friendcount;//限制friend插件次数
    vector<RoomItem *>vec_roomlist;
    vector<FriendItem*>vec_frienditem;
    vector<FriendItem*>vec_friendlist;
    vector<RoomItem *>vec_roomitem;
    MyPushButton *startgame;
    MyPushButton *startgame1;
    vector<int>m_vecId;
public slots:
    void SLOT_DealLoginRs(char *buf,int nlen);//处理登录回复槽函数
    void SLOT_DealRegisterRs(char *buf,int nlen);//处理注册回复槽函数
    void SLOT_DealAskRoomRs(char *buf,int nlen);//处理请求房间回复槽函数
    void SLOT_DealAlterInfoRs(char *buf,int nlen);//处理修改信息回复槽函数
    void SLOT_DealCreateRoom(char *buf,int nlen);
    void SLOT_DealSearchFriend(char* buf,int nlen);
    void SLOT_DealAddFriend(char *buf,int nlen);
    void SLOT_ReadyData(char* buf,int nlen);//接收包槽函数
    void SLOT_DealAddFriendRq(char *buf,int nlen);//好友添加
    void SLOT_DealGetFriendListRs(char *buf,int nlen);//获取好友列表
    void SLOT_DealSearchRoomRs(char *buf,int nlen);//处理查找房间
    void SLOT_DealJoinRoomRs(char *buf,int nlen);//处理加入房间
    void SLOT_DealStartGameRs(char *buf,int nlen);//处理开始游戏
    void SLOT_DealPostIdentity(char *buf,int nlen);
    void SLOT_ReGetFriendList();//刷新好友列表
    void SLOT_ShowAlterInfo();//显示更改信息窗口的槽函数
    void SLOT_ShowCreateRoom();//显示创建房间窗口的槽函数
    void SLOT_ShowAddFriend();//显示添加好友窗口的槽函数
    void SLOT_ShowJoinRoom();//显示查找房间窗口的槽函数
    void SLOT_ReGetRoomTable();//刷新房间列表槽函数
    void SLOT_GetFriendList();//显示好友列表窗口槽函数
    void SLOT_SetCountZero();

};

#endif // CKERNEL_H
