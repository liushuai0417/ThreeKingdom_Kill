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
#include"herobutton.h"
#include<map>
#include"cardbutton.h"
#include<QLabel>
#include<queue>
#include"showothercard.h"
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
    QString GetHeroPath(int chooseheroid);//获取选择英雄的路径
    void InitCard();//初始化手牌
    QString GetColorPath(int colorid);//获取花色路径
    QString GetNumPath(int num);//获取点数路径
    void ShowZgPosition();//显示主公位置
    void ShowHero();//显示所有人选择的英雄
    void ShowZGIdentity(int mark,int id,int seatid);//显示所有人的身份牌
    int FindSeatIdById(int myid);//通过用户id查找座位id
    void ShowHp();//显示所有人的血量
    QString GetCardName(int cardid);//通过牌的id获取牌名
    QString GetCardPath(int cardid);//通过id获取牌的路径
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
    ShowOtherCard *showothercarddlg;
    MyPushButton *identity;//身份牌
    MyPushButton *chupai;
    MyPushButton *qipai;
    MyPushButton *queding;
    QLabel *label;//提示标签
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
    vector<RoomItem *>vec_roomlist;//房间列表的vector
    vector<FriendItem*>vec_frienditem;//好友信息插件的vector
    vector<FriendItem*>vec_friendlist;//好友表的vector
    vector<RoomItem *>vec_roomitem;//房间信息插件的vector
    MyPushButton *startgame;//房主的准备按钮
    MyPushButton *startgame1;//其他成员的准备按钮
    vector<int>m_vecId;//房间成员的id数组
    vector<HeroButton*>vec_hero;//拷贝的选择英雄数组
    vector<CardButton*>vec_card;//手牌数组
    vector<CardButton*>vec_pushcard;//自己打出的手牌数组
    vector<CardButton*>vec_otherpushcard;//别人打出的手牌数组
    HeroButton *myhero;//我的英雄牌
    MyPushButton *identityattention;//身份提示
    map<int,vector<int>>m_mapIdtoHeroId;//用户id和英雄的映射
    map<int,int>m_mapSeatIdToHeroId;//座位号和英雄id的映射
    map<int,int>m_mapSeatIdToHp;//座位号和血量的映射
    map<int,int>m_mapSeatIdToId;//用户id和座位号的映射
    map<int,vector<int>>m_mapSeatIdToPosition;//座位号和位置坐标的映射
    map<int,int>m_mapSeatIdToLength;//座位号和距离的映射
    queue<CardButton*>m_queQuitCard;//弃牌队列
    int chooseid;//选择英雄的下标
    int m_roomid;//房间id
    int m_identity;//我的身份
    int heroid[6];//选择英雄的编号数组
    vector<int>card;//初始手牌编号数组
    int ZG_Id;//主公的账户id
    int ZG_HeroId;//主公的英雄id
    int My_HeroId;//我的英雄id
    int MySeatId;//我的座位号
    QString myHeroPath;
    MyPushButton *turnlogo;//回合标识
    MyPushButton *killaction;//动画效果
    MyPushButton *queren;//确认按钮
    int usecardtoid1;//出牌对象1
    int usecardtoid2;//出牌对象2
    STRU_CARD choosecard;//要打出的手牌结构体
    int cardnum;//手牌数量
    int myhp;//我的血量
    CardButton *pushCard;//打出的牌
    int b_choosefirstpeople;//是否选择了第一个人
    bool b_flagpush;//是否主动出牌
    bool b_isKill;//是否出过杀
    int yuserid;//被使用牌的id
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
    void SLOT_DealSelectHero(char *buf,int nlen);//选择英雄
    void SLOT_ReGetFriendList();//刷新好友列表
    void SLOT_ShowAlterInfo();//显示更改信息窗口的槽函数
    void SLOT_ShowCreateRoom();//显示创建房间窗口的槽函数
    void SLOT_ShowAddFriend();//显示添加好友窗口的槽函数
    void SLOT_ShowJoinRoom();//显示查找房间窗口的槽函数
    void SLOT_ReGetRoomTable();//刷新房间列表槽函数
    void SLOT_GetFriendList();//显示好友列表窗口槽函数
    void SLOT_SetCountZero();
    void SLOT_DealAllSelHeroRs(char *buf,int nlen);//处理返回所有人选择的英雄和自身用户id
    void SLOT_DealGetCardRs(char *buf,int nlen);//处理抽卡回复
    void SLOT_DealRoomMemberRs(char *buf,int nlen);//更新房间成员回复
    void SLOT_DealTurnBeginRs(char *buf,int nlen);//处理回合开始
    void SLOT_DealPostCardRs(char *buf,int nlen);//处理出牌回复
    void SLOT_DealReposeCardRq(char *buf,int nlen);//处理请求出牌
    void SLOT_CommitStatus(char *buf,int nlen);//同步血量
    void SLOT_OffCardRq(char *buf,int nlen);//同步弃牌动画
    void SLOT_GHCQ_Rq(char *buf,int nlen);//处理过河拆桥
    void SLOT_GHCQ_Rs(char *buf,int nlen);//处理过河拆桥回复
    void SLOT_SSQY_Rq(char *buf,int nlen);//处理顺手牵羊请求
    void SLOT_SSQY_Rs(char *buf,int nlen);//处理顺手牵羊回复
};

#endif // CKERNEL_H
