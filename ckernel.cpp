#include "ckernel.h"
#include"packdef.h"
#include<QDebug>
#include<QMessageBox>
#include<QLatin1String>
#include<QThread>
#include "ui_dialog.h"
#include"roomitem.h"
#include "ui_mainscene.h"
#include"ui_roomitem.h"
#include"addfrienddialog.h"
#include"frienditem.h"
#include"mypushbutton.h"
#include<QPixmap>
#include"gamingdialog.h"
#include<QPushButton>
#include<QTimer>
#include"herobutton.h"
#define NetPackMap(a) m_NetPackMap[(a)-DEF_PACK_BASE]
CKernel::CKernel(QObject *parent) : QObject(parent)
{
    //初始化指针
    m_tcpClient = new QMyTcpClient;//网络指针
    m_Dialog = new Dialog;//登录注册指针
    m_Dialog->show();
    m_MainScene = new MainScene;//大厅指针
    changeDialog = new ChanegInfoDialog;//修改个人信息窗口指针
    createDialog = new CreateRoomDialog;//创建房间窗口指针
    addDialog = new AddFriendDialog;//添加好友窗口指针
    joinDialog = new JoinRoomDialog;//加入房间窗口指针
    gamingdlg = new GamingDialog;
    friendlistDialog = new FriendList;
    item = new RoomItem;
    chooseid = -1;
    m_roomcount = 0;
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
    void (ChanegInfoDialog::*AlterInfoSignal)(int,QString,QString) = &ChanegInfoDialog::SIG_AleterInfoCommit;
    void (CreateRoomDialog::*CreateRoomSignal)(QString) = &CreateRoomDialog::SIG_CreateRoomCommit;
    void (AddFriendDialog::*AddFriendByNameSignal)(QString) = &AddFriendDialog::SIG_AddFriendByNameCommit;
    void (JoinRoomDialog::*JoinRoomByNameSignal)(QString) = &JoinRoomDialog::SIG_JoinRoomByNameCommit;
    void (JoinRoomDialog::*JoinRoomByIdSignal)(QString) = &JoinRoomDialog::SIG_JoinRoomByIdCommit;
    void (GamingDialog::*LeaveRoomSignal)(int) = &GamingDialog::SIG_LeaveRoom;
    void (RoomItem::*JoinRoom)(QString) = &RoomItem::SIG_JoinRoom;
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

    //修改信息的槽函数
    connect(changeDialog,AlterInfoSignal,[=](int iconid,QString name,QString feeling){
        //发送修改信息的包
        qDebug()<<"发送修改信息";
        qDebug()<<iconid<<name<<feeling;
        STRU_ALTER_USERINFO_RQ rq;
        rq.m_userInfo.m_userid = this->m_id;
        rq.m_userInfo.m_iconid = iconid;
        strcpy(rq.m_userInfo.m_szName,name.toStdString().c_str());
        strcpy(rq.m_userInfo.m_szFelling,feeling.toStdString().c_str());
        this->m_iconID = iconid;
        this->m_szName = name;
        this->m_feeling = feeling;
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
    });

    //创建房间的槽函数
    connect(createDialog,CreateRoomSignal,[=](QString roomname){
        qDebug()<<"发送创建房间";
        STRU_CREATEROOM_RQ rq;
        strcpy(rq.m_RoomName,roomname.toStdString().c_str());
        strcpy(rq.m_szUser,this->m_szName.toStdString().c_str());
        rq.m_userid = this->m_id;
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
        createDialog->hide();
    });

    //通过name查找好友
    connect(addDialog,AddFriendByNameSignal,[=](QString content){
        //发送查找好友包
        STRU_SEARCH_FRIEND_RQ rq;
        strcpy(rq.sz_friendName,content.toStdString().c_str());
        rq.m_userid = this->m_id;
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
    });

    //通过name查找房间
    connect(joinDialog,JoinRoomByNameSignal,[=](QString content){
        //发送查找房间包
        //如果有清空当前显示的插件
        auto ite = joinDialog->vec.begin();
        while(ite!=joinDialog->vec.end()){
            joinDialog->Slot_RemoveRoomItem(*ite);
            delete *ite;
            *ite = NULL;
            ite++;
        }
        STRU_SEARCH_ROOM_RQ rq;
        strcpy(rq.m_szRoomName,content.toStdString().c_str());
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
        m_roomcount++;
    });

    //通过id查找房间
    connect(joinDialog,JoinRoomByIdSignal,[=](QString content){
        //发送查找房间包
        //如果有清空当前显示的插件
        auto ite = joinDialog->vec.begin();
        while(ite!=joinDialog->vec.end()){
            joinDialog->Slot_RemoveRoomItem(*ite);
            delete *ite;
            *ite = NULL;
            ite++;
        }
        STRU_SEARCH_ROOM_RQ rq;
        rq.m_Roomid = content.toInt();
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
        m_roomcount++;
    });

    //双击加入房间
    connect(item,JoinRoom,[=](QString roomid){
        qDebug()<<__func__<<roomid;
        STRU_SEARCH_ROOM_RQ rq;
        rq.m_Roomid = roomid.toInt();
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
    });
    //离开房间槽函数
    connect(gamingdlg,LeaveRoomSignal,[=](int roomid){
        STRU_LEAVEROOM_RQ rq;
        rq.m_RoomId = roomid;
        rq.m_userId = this->m_id;
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
        gamingdlg->hide();
        identity->hide();
        delete identity;
        m_MainScene->show();
    });
    //加入房间槽函数

    connect(m_MainScene,&MainScene::SIG_ReGetRoomTable,this,&CKernel::SLOT_ReGetRoomTable);
    connect(m_MainScene,&MainScene::SIG_ShowAlterInfo,this,&CKernel::SLOT_ShowAlterInfo);
    connect(m_MainScene,&MainScene::SIG_CreateRoom,this,&CKernel::SLOT_ShowCreateRoom);
    connect(m_MainScene,&MainScene::SIG_AddFriend,this,&CKernel::SLOT_ShowAddFriend);
    connect(m_MainScene,&MainScene::SIG_JoinRoom,this,&CKernel::SLOT_ShowJoinRoom);
    connect(m_MainScene,&MainScene::SIG_GetFriendList,this,&CKernel::SLOT_GetFriendList);
    connect(friendlistDialog,&FriendList::SIG_ReGetFriendList,this,&CKernel::SLOT_ReGetFriendList);
    connect(joinDialog,&JoinRoomDialog::SIG_SetCountZero,this,&CKernel::SLOT_SetCountZero);

    //房间列表的第一行
    RoomItem *itemindex = new RoomItem;
    m_MainScene->Slot_AddUserItem(itemindex);
}

void CKernel::SLOT_SetCountZero(){
    this->m_roomcount = 0;
}

void CKernel::SLOT_ReGetFriendList(){
    auto ite = vec_friendlist.begin();
    while(ite!=vec_friendlist.end()){
        m_MainScene->Slot_RemoveUserItem(*ite);
        delete *ite;
        *ite = NULL;
        ite++;
    }
    m_MainScene->repaint();
    STRU_GETFRILIST_RQ rq;
    rq.m_userid = this->m_id;
    m_tcpClient->SendData((char*)&rq,sizeof(rq));
}

//接收数据槽函数
void CKernel::SLOT_ReadyData(char *buf,int nlen){
    //获取包类型
    int type = *(int*)buf;
    qDebug()<<type;
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
    NetPackMap(DEF_PACK_CREATEROOM_RS) = &CKernel::SLOT_DealCreateRoom;
    NetPackMap(DEF_PACK_SEARCH_FRIEND_RS) = &CKernel::SLOT_DealSearchFriend;
    NetPackMap(DEF_PACK_ADD_FRIEND_RS) = &CKernel::SLOT_DealAddFriend;
    NetPackMap(DEF_PACK_ADD_FRIEND_RQ) = &CKernel::SLOT_DealAddFriendRq;
    NetPackMap(DEF_ALTER_USERINFO_RS) = &CKernel::SLOT_DealAlterInfoRs;
    NetPackMap(DEF_PACK_GETFRIENDLIST_RS) = &CKernel::SLOT_DealGetFriendListRs;
    NetPackMap(DEF_PACK_SEARCH_ROOM_RS) = &CKernel::SLOT_DealSearchRoomRs;
    NetPackMap(DEF_PACK_JOINROOM_RS) = &CKernel::SLOT_DealJoinRoomRs;
    NetPackMap(DEF_PACK_STARTGAME_RS) = &CKernel::SLOT_DealStartGameRs;
    NetPackMap(DEF_PACK_POST_IDENTITY) = &CKernel::SLOT_DealPostIdentity;
    NetPackMap(DEF_PACK_SELHERO_RQ) = &CKernel::SLOT_DealSelectHero;
    NetPackMap(DEF_PACK_ALLSELHERO_RS) = &CKernel::SLOT_DealAllSelHeroRs;
    NetPackMap(DEF_PACK_GETCARD_RS) = &CKernel::SLOT_DealGetCardRs;
}

//处理抽卡回复
void CKernel::SLOT_DealGetCardRs(char *buf,int nlen){
    STRU_GETCARD_RS* rs = (STRU_GETCARD_RS*)buf;
    for(int i=0;i<sizeof(rs->m_card)/sizeof(rs->m_card[0]);i++){
        qDebug()<<rs->m_card[i].id;
    }
}

//处理返回所有人选择的英雄和自身用户id
void CKernel::SLOT_DealAllSelHeroRs(char *buf,int nlen){
    STRU_ALLSEL_HERO_RS *rs = (STRU_ALLSEL_HERO_RS *)buf;
    for(int i=0;i<sizeof(rs->heroarr)/sizeof(rs->heroarr[0]);i++){
        this->m_mapIdtoHeroId[rs->user_idarr[i]] = this->m_mapIdtoHeroId[rs->heroarr[i]];
    }
    //发送抽卡请求
    STRU_GETCARD_RQ rq;
    if(this->m_identity == zhugong){
        rq.num = 6;
    }else{
        rq.num = 4;
    }
    rq.m_roomid = this->m_roomid;
    rq.m_userid = this->m_id;
}

//处理选择英雄
void CKernel::SLOT_DealSelectHero(char *buf,int nlen){
    STRU_SELHERO_RQ *rq = (STRU_SELHERO_RQ*)buf;
    if(this->m_identity != zhugong){
        this->ZG_HeroId = rq->ZG_heroid;
    }

    for(int i=0;i<sizeof(rq->m_HeroArr)/sizeof(rq->m_HeroArr[0]);i++){
        heroid[i] = rq->m_HeroArr[i];
        if(rq->m_HeroArr[i]!=-1){
            HeroButton *hero;
        switch(rq->m_HeroArr[i]){
        case huatuo:
            hero = new HeroButton(":/res/YX/华佗.png");
            break;
        case diaochan:
            hero = new HeroButton(":/res/YX/貂蝉.png");
            break;
        case lvbu:
            hero = new HeroButton(":/res/YX/吕布.png");
            break;
        case guanyu:
            hero = new HeroButton(":/res/YX/关羽.png");
            break;
        case zhangfei:
            hero = new HeroButton(":/res/YX/张飞.png");
            break;
        case zhaoyun:
            hero = new HeroButton(":/res/YX/赵云.png");
            break;
        case huangyueying:
            hero = new HeroButton(":/res/YX/黄月英.png");
            break;
        case zhugeliang:
            hero = new HeroButton(":/res/YX/诸葛亮.png");
            break;
        case machao:
            hero = new HeroButton(":/res/YX/马超.png");
            break;
        case simayi:
            hero = new HeroButton(":/res/YX/司马懿.png");
            break;
        case xiahoudun:
            hero = new HeroButton(":/res/YX/夏侯惇.png");
            break;
        case xuchu:
            hero = new HeroButton(":/res/YX/许褚.png");
            break;
        case guojia:
            hero = new HeroButton(":/res/YX/郭嘉.png");
            break;
        case zhangliao:
            hero = new HeroButton(":/res/YX/张辽.png");
            break;
        case liubei:
            hero = new HeroButton(":/res/YX/刘备.png");
            break;
        case caocao:
            hero = new HeroButton(":/res/YX/曹操.png");
            break;
        case sunquan:
            hero = new HeroButton(":/res/YX/孙权.png");
            break;
        default:
            hero = new HeroButton(":/res/YX/吕蒙.png");
            break;
        }

        connect(hero,&HeroButton::clicked,[=]()mutable{
            if(hero->b_flagchoose){
                hero->ChooseHero1();
            }else{
                hero->ChooseHero();
            }
            hero->chooseheroid = i;
            chooseid = i;
            //获取路径名
            this->myHeroPath = GetHeroPath(this->heroid[i]);
            hero->b_flagchoose = !hero->b_flagchoose;
        });
        vec_hero.push_back(hero);
        hero->setParent(gamingdlg);
        hero->move(gamingdlg->width()*0.5-hero->width()*0.5-50+i*140,gamingdlg->height()*0.8-10);
        hero->show();
        gamingdlg->update();
        }
    }
    MyPushButton *chooseheroattention = new MyPushButton(":/res/BJ/提示_选择英雄.png");
    chooseheroattention->setParent(gamingdlg);
    MyPushButton *choosehero = new MyPushButton(":/res/icon/choosehero.png",":/res/icon/choosehero_1.png");
    choosehero->setParent(gamingdlg);
    QTimer::singleShot(6000,this,[=]{
        chooseheroattention->move(gamingdlg->width()*0.5-chooseheroattention->width(),gamingdlg->height()*0.5);
        chooseheroattention->show();
        choosehero->move(gamingdlg->width()*0.5-choosehero->width()*0.5+100,gamingdlg->height()*0.5);
        choosehero->show();
        gamingdlg->update();
    });

    connect(choosehero,&MyPushButton::clicked,[=](){
        chooseheroattention->hide();
        STRU_SELHERO_RS rs;
        rs.room_id = this->m_roomid;
        rs.user_id = this->m_id;
        if(this->m_identity == zhugong){
            rs.isZG = true;
        }else{
            rs.isZG = false;
        }
        rs.hero_id = this->heroid[chooseid];
        this->My_HeroId = this->heroid[chooseid];
        if(this->m_identity == zhugong){
            this->ZG_HeroId = this->My_HeroId;
        }
        m_tcpClient->SendData((char*)&rs,sizeof(rs));
        auto ite = vec_hero.begin();
        while(ite != vec_hero.end()){
            (*ite)->hide();
            ++ite;
        }
        choosehero->hide();
        myhero = new HeroButton(this->myHeroPath,"");
        myhero->setParent(gamingdlg);
        myhero->move(gamingdlg->width()*0.5-myhero->width()*0.5-50+140,gamingdlg->height()*0.8-10);
        myhero->show();
        gamingdlg->update();
    });
}

//处理选择身份
void CKernel::SLOT_DealPostIdentity(char *buf,int nlen){
    STRU_POST_IDENTITY *rs = (STRU_POST_IDENTITY*)buf;
    this->m_identity = rs->m_identity;
    this->ZG_Id = rs->m_ZG_userid;
    switch(rs->m_identity){
        case zhugong:
        {
            identity = new MyPushButton(":/res/Shenfen/主公.png");
            identityattention = new MyPushButton(":/res/BJ/提示_主公.png");
        }
        break;
        case zhongchen:
        {
            identity = new MyPushButton(":/res/Shenfen/忠臣.png");
            identityattention = new MyPushButton(":/res/BJ/提示_忠臣.png");
        }
        break;
        case neijian:
        {
            identity = new MyPushButton(":/res/Shenfen/内奸.png");
            identityattention = new MyPushButton(":/res/BJ/提示_内奸.png");
        }
        break;
        case fanzei:
        {
            identity = new MyPushButton(":/res/Shenfen/反贼.png");
            identityattention = new MyPushButton(":/res/BJ/提示_反贼.png");
        }
        break;
    }
        identity->setParent(gamingdlg);
        identity->move(gamingdlg->width()*0.5-identity->width()*0.5-250,gamingdlg->height()*0.8-10);
        identity->show();
        gamingdlg->update();

        QTimer::singleShot(2000,this,[=]{
            identityattention->setParent(gamingdlg);
            identityattention->move(gamingdlg->width()*0.5-identityattention->width(),gamingdlg->height()*0.5);
            identityattention->show();
            gamingdlg->update();
        });

        QTimer::singleShot(4000,this,[=]{
            identityattention->hide();
            delete identityattention;
            gamingdlg->update();
        });

}

//获取选择英雄的路径
QString CKernel::GetHeroPath(int chooseheroid){
    QString result;
    switch(chooseheroid){
    case huatuo:
        result = QString(":/res/YX/华佗.png");
        break;
    case diaochan:
        result = QString(":/res/YX/貂蝉.png");
        break;
    case lvbu:
        result = QString(":/res/YX/吕布.png");
        break;
    case guanyu:
        result = QString(":/res/YX/关羽.png");
        break;
    case zhangfei:
        result = QString(":/res/YX/张飞.png");
        break;
    case zhaoyun:
        result = QString(":/res/YX/赵云.png");
        break;
    case huangyueying:
        result = QString(":/res/YX/黄月英.png");
        break;
    case zhugeliang:
        result = QString(":/res/YX/诸葛亮.png");
        break;
    case machao:
        result = QString(":/res/YX/马超.png");
        break;
    case simayi:
        result = QString(":/res/YX/司马懿.png");
        break;
    case xiahoudun:
        result = QString(":/res/YX/夏侯惇.png");
        break;
    case xuchu:
        result = QString(":/res/YX/许褚.png");
        break;
    case guojia:
        result = QString(":/res/YX/郭嘉.png");
        break;
    case zhangliao:
        result = QString(":/res/YX/张辽.png");
        break;
    case liubei:
        result = QString(":/res/YX/刘备.png");
        break;
    case caocao:
        result = QString(":/res/YX/曹操.png");
        break;
    case sunquan:
        result = QString(":/res/YX/孙权.png");
        break;
    default:
        result = QString(":/res/YX/吕蒙.png");
        break;
    }
    return result;
}

void CKernel::SLOT_DealStartGameRs(char *buf,int nlen){
    STRU_STARTGAME_RS *rs = (STRU_STARTGAME_RS*)buf;
    switch(rs->m_lResult){
        case game_start_success:
            {
                if(startgame){
                    startgame->hide();
                    delete startgame;
                    startgame = NULL;
                }
                if(startgame1){
                    startgame1->hide();
                    delete startgame1;
                    startgame1 = NULL;
                }
            }
        break;
    }
}

//处理加入房间
void CKernel::SLOT_DealJoinRoomRs(char *buf,int nlen){
    STRU_JOINROOM_RS *rs = (STRU_JOINROOM_RS *)buf;
    switch(rs->m_lResult){
        case room_no_exist:
            QMessageBox::about(joinDialog,"提示","该房间不存在");
        break;
        case room_is_full:
            QMessageBox::about(joinDialog,"提示","该房间满员");
        break;
        case join_success:
            {
                //座位号赋值
                qDebug()<<rs->place;
                this->MySeatId = rs->place;
//                int i=0;
//                m_vecId.clear();
//                rs->m_userInfoarr[i].m_userid;
//                while(rs->m_userInfoarr[i].m_userid != 0){
//                    if(rs->m_userInfoarr[i].m_userid!=this->m_id){
//                        m_vecId.push_back(rs->m_userInfoarr[i].m_userid);
//                    }
//                    i++;
//                }
            }
        break;
    }
}

//处理查找房间回复
void CKernel::SLOT_DealSearchRoomRs(char *buf,int nlen){
    STRU_SEARCH_ROOM_RS *rs = (STRU_SEARCH_ROOM_RS *)buf;
    switch(rs->m_lResult){
        case search_room_success:
        {
            QString roomname = rs->m_roomInfo.sz_Roomname;
            int roomid = rs->m_roomInfo.m_Roomid;
            QString roomcreator = rs->m_roomInfo.sz_RoomCreator;
            int num = rs->m_roomInfo.m_num;

            if(this->m_roomcount==1){
                item = new RoomItem;
                item->setItem(roomid,roomname,roomcreator,num);
                joinDialog->vec.push_back(item);
                joinDialog->Slot_AddRoomItem(item);
                MyPushButton *addRoom = new MyPushButton(":/res/icon/addfriend.png",":/res/icon/addfriend_1.png");
                addRoom->setParent(item);
                addRoom->move(item->width()-80,item->height()*0.8-25);
                connect(addRoom,&MyPushButton::clicked,[=](){
                    int roomid = item->getUi()->lb_roomid->text().toInt();
                    STRU_JOINROOM_RQ rq;
                    rq.m_RoomID = roomid;
                    rq.m_userInfo.m_iconid = this->m_iconID;
                    this->m_roomid = roomid;
                    strcpy(rq.m_userInfo.m_szFelling,this->m_feeling.toStdString().c_str());
                    strcpy(rq.m_userInfo.m_szName,this->m_szName.toStdString().c_str());
                    rq.m_userInfo.m_userid = this->m_id;
                    m_tcpClient->SendData((char*)&rq,sizeof(rq));
                    gamingdlg->roomid = roomid;
                    startgame1 = new MyPushButton(":/res/icon/btnzhunbei.png",":/res/icon/btnzhunbei_1.png");
                    startgame1->setParent(gamingdlg);
                    startgame1->move(gamingdlg->width()*0.5-startgame1->width()*0.5,gamingdlg->height()*0.8-10);
                    m_MainScene->hide();
                    joinDialog->hide();
                    gamingdlg->setGeometry(m_MainScene->geometry());
                    gamingdlg->show();
                    connect(startgame1,&MyPushButton::clicked,[=](){
                        STRU_STARTGAME_RQ rq;
                        rq.Room_id = gamingdlg->roomid;
                        rq.user_id = this->m_id;
                        startgame1->setIcon(QIcon(":/res/icon/yizhunbei.png"));
                        m_tcpClient->SendData((char*)&rq,sizeof(rq));
                    });
                });

            }

        }
        break;
        case search_room_failed:
        {
            QMessageBox::about(joinDialog,"提示","该房间不存在");
        }
        break;
    }
}


//处理刷新好友列表回复
void CKernel::SLOT_DealGetFriendListRs(char *buf,int nlen){
    STRU_GETFRILIST_RS *rs = (STRU_GETFRILIST_RS *)buf;
    switch(rs->m_lResult){
        case ask_success:
        {
            int i=0;
            while(rs->m_friArr[i].m_userid !=0){
                FriendItem *item = new FriendItem;
                item->setItem(rs->m_friArr[i].m_iconid,rs->m_friArr[i].m_szName,rs->m_friArr[i].m_szFelling,rs->m_friArr[i].status);
                this->vec_friendlist.push_back(item);
                friendlistDialog->Slot_AddFriendItem(item);
                i++;
            }
            friendlistDialog->show();
        }
        break;
        case aks_failed:
        {
            QMessageBox::about(m_Dialog,"提示","获取好友列表失败");
        }
        break;
    }
}

//处理修改信息回复
void CKernel::SLOT_DealAlterInfoRs(char *buf,int nlen){
    STRU_ALTER_USERINFO_RS *rs = (STRU_ALTER_USERINFO_RS *)buf;
    switch(rs->m_result){
        case name_repeat:
            {
               QMessageBox::about(m_Dialog,"提示","修改的用户名已存在");
            }
        break;
        case alter_success:
        {
            //修改控件
            QMessageBox::about(m_Dialog,"提示","修改成功");
            changeDialog->hide();
            m_MainScene->getUi()->lb_name->setText(m_szName);
            m_MainScene->getUi()->lb_feeling->setText(m_feeling);
            QPixmap pix;
            QString strPath;
            if(m_iconID<10){
                strPath = QString(":/res/TX/0%1.png").arg(m_iconID+1);
            }else{
                strPath = QString(":/res/TX/%1.png").arg(m_iconID+1);
            }
            pix.load(strPath);

            m_MainScene->getUi()->pb_icon->setFixedSize(pix.width(),pix.height());
            m_MainScene->getUi()->pb_icon->setStyleSheet("QPushButton{border:0px;}");//边框设置为0像素
            m_MainScene->getUi()->pb_icon->setIcon(pix);
            m_MainScene->getUi()->pb_icon->setIconSize(QSize(pix.width(),pix.height()));
        }
        break;
    }
}

//处理好友添加请求
void CKernel::SLOT_DealAddFriendRq(char *buf,int nlen){
    STRU_ADD_FRIEND_RQ *rq = (STRU_ADD_FRIEND_RQ *)buf;
    QString friendname = QString(rq->m_friInfo.m_szName);

    QString str = QString("[%1]请求添加你为好友,是否同意?").arg(friendname);
    STRU_ADD_FRIEND_RS rs;
    rs.m_friInfo.m_userid = this->m_id;
    strcpy(rs.m_friInfo.m_szFelling,this->m_feeling.toStdString().c_str());
    strcpy(rs.m_friInfo.m_szName,this->m_szName.toStdString().c_str());
    rs.m_friInfo.m_iconid = this->m_iconID;
    if(QMessageBox::question(addDialog,"添加好友",str) == QMessageBox::Yes){
        rs.m_result = add_success;
    }else{
        rs.m_result = add_failed;
    }
    FriendItem *item = new FriendItem;
    item->setItem(rq->m_userInfo.m_iconid,rq->m_userInfo.m_szName,rq->m_userInfo.m_szFelling,rq->m_userInfo.status);
    vec_frienditem.push_back(item);
    friendlistDialog->Slot_AddFriendItem(item);
    m_tcpClient->SendData((char*)&rs,sizeof(rs));
}

//处理好友添加回复
void CKernel::SLOT_DealAddFriend(char *buf,int nlen){
    STRU_ADD_FRIEND_RS *rs = (STRU_ADD_FRIEND_RS *)buf;
    qDebug()<<"SLOT_DealAddFriend";
    if(rs->m_result == add_success){
        QMessageBox::about(m_Dialog,"提示","该用户同意请求");
    }else{
        QMessageBox::about(m_Dialog,"提示","该用户拒绝请求");
    }

    m_tcpClient->SendData((char*)&rs,sizeof(rs));
}

//处理查找好友回复
void CKernel::SLOT_DealSearchFriend(char *buf,int nlen){
    STRU_SEARCH_FRIEND_RS *rs = (STRU_SEARCH_FRIEND_RS *)buf;
    int iconid;
    int state;
    QString name;
    QString feeling;
    switch(rs->m_result){
        case no_this_user:
           QMessageBox::about(m_Dialog,"提示","用户不存在");
        break;
        case is_your_friend:
            QMessageBox::about(m_Dialog,"提示","该用户是你的好友");
        break;
        case search_success:
        {
            qDebug()<<"查询结果"<<rs->m_friInfo.m_szName;
            iconid = rs->m_friInfo.m_iconid;
            state = rs->m_friInfo.status;
            name = QString(rs->m_friInfo.m_szName);
            feeling = QString(rs->m_friInfo.m_szFelling);
            FriendItem *item = new FriendItem;
            item->setItem(iconid,name,feeling,state);
            addDialog->vec.push_back(item);
            MyPushButton *addFriend = new MyPushButton(":/res/icon/addfriend.png",":/res/icon/addfriend_1.png");
            addFriend->setParent(item);
            addFriend->move(item->width()-80,item->height()*0.8-25);
            item->m_friendName = QString(rs->m_friInfo.m_szName);
            item->m_friendId = rs->m_friInfo.m_userid;
            connect(addFriend,&MyPushButton::clicked,[&](){
                STRU_ADD_FRIEND_RQ rq;
                //个人信息初始化
                rq.m_userInfo.m_userid = this->m_id;
                strcpy(rq.m_userInfo.m_szFelling,this->m_feeling.toStdString().c_str());
                strcpy(rq.m_userInfo.m_szName,this->m_szName.toStdString().c_str());
                rq.m_userInfo.m_iconid = this->m_iconID;
                rq.m_userInfo.status = this->m_state;
                //好友信息初始化
                rq.m_friInfo.m_iconid = iconid;
                strcpy(rq.m_friInfo.m_szFelling,feeling.toStdString().c_str());
                strcpy(rq.m_friInfo.m_szName,name.toStdString().c_str());
                rq.m_friInfo.m_userid = rs->m_friInfo.m_userid;
                rq.m_friInfo.status = rs->m_friInfo.status;
                m_tcpClient->SendData((char*)&rq,sizeof(rq));
            });

            addDialog->Slot_AddFriendItem(item);
        }
        break;
    }
}


//处理登录回复槽函数
void CKernel::SLOT_DealLoginRs(char *buf,int nlen){
    //拆包
    STRU_LOGIN_RS *rs = (STRU_LOGIN_RS *)buf;
    //判断请求包的结果
    switch(rs->m_lResult){
        case login_sucess:
        {
            //初始化个人信息

            this->m_id = rs->m_userInfo.m_userid;
            this->m_iconID = rs->m_userInfo.m_iconid;
            this->m_szName = QString(rs->m_userInfo.m_szName);
            this->m_feeling = QString(rs->m_userInfo.m_szFelling);
            this->m_state = rs->m_userInfo.status;
            m_MainScene->getUi()->lb_name->setText(m_szName);
            m_MainScene->getUi()->lb_feeling->setText(m_feeling);
            QPixmap pix;
            QString strPath;
            if(m_iconID<10){
                strPath = QString(":/res/TX/0%1.png").arg(m_iconID+1);
            }else{
                strPath = QString(":/res/TX/%1.png").arg(m_iconID+1);
            }
            pix.load(strPath);
            m_MainScene->getUi()->pb_icon->setFixedSize(pix.width(),pix.height());
            m_MainScene->getUi()->pb_icon->setStyleSheet("QPushButton{border:0px;}");//边框设置为0像素
            m_MainScene->getUi()->pb_icon->setIcon(pix);
            m_MainScene->getUi()->pb_icon->setIconSize(QSize(pix.width(),pix.height()));
            //登录成功
            QMessageBox::about(m_Dialog,"提示","登录成功");
            m_Dialog->hide();
            //发送查询房间列表的包
            STRU_ASKROOM_RQ rq;
            m_tcpClient->SendData((char*)&rq,sizeof(rq));
            m_MainScene->setGeometry(m_Dialog->geometry());
            m_MainScene->show();
        }
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

//处理获取房间列表回复
void CKernel::SLOT_DealAskRoomRs(char *buf,int nlen){

    STRU_ASKROOM_RS *rs = (STRU_ASKROOM_RS *)buf;
    switch(rs->m_lResult){
        case ask_room_success:
        {
            int i=0;
            while(rs->m_RoomList[i].m_Roomid!=0){
                QString roomname = QString(rs->m_RoomList[i].sz_Roomname);
                int roomid = rs->m_RoomList[i].m_Roomid;
                QString roomcreator = QString(rs->m_RoomList[i].sz_RoomCreator);
                int number = rs->m_RoomList[i].m_num;
                item = new RoomItem;
                vec_roomlist.push_back(item);
                item->setItem(roomid,roomname,roomcreator,number);
                m_MainScene->Slot_AddUserItem(item);
                i++;
            }

        }
        break;
    }
}

//处理创建房间回复
void CKernel::SLOT_DealCreateRoom(char *buf,int nlen){
    STRU_CREATEROOM_RS *rs = (STRU_CREATEROOM_RS *)buf;
    QString str;
    switch (rs->m_lResult) {
        case create_success:{
            str = QString("创建房间成功,房间号为%1").arg(rs->m_RoomId);
            this->MySeatId = 1;
            this->m_roomid  = rs->m_RoomId;
            QMessageBox::about(m_MainScene,"提示",str);
            startgame = new MyPushButton(":/res/icon/btnzhunbei.png");
            startgame->setParent(gamingdlg);
            startgame->move(gamingdlg->width()*0.5-startgame->width()*0.5,gamingdlg->height()*0.8-10);
            gamingdlg->roomid = rs->m_RoomId;
            m_MainScene->hide();
            gamingdlg->setGeometry(m_MainScene->geometry());
            gamingdlg->show();
            static bool flag = true;
            connect(startgame,&MyPushButton::clicked,[&](){
                STRU_STARTGAME_RQ rq;
                rq.Room_id = gamingdlg->roomid;
                rq.user_id = this->m_id;
                if(flag){
                    QPixmap pix;
                    pix.load(":/res/icon/yizhunbei.png");
                    startgame->setIcon(pix);
                }
                if(!flag){
                    QPixmap pix;
                    pix.load(":/res/icon/btnzhunbei.png");
                    startgame->setIcon(pix);
                }
                m_tcpClient->SendData((char*)&rq,sizeof(rq));
                flag = !flag;
            });
        }
        break;
        case create_failed:
            QMessageBox::about(m_MainScene,"提示","创建房间失败");
            break;
    }
}
//显示更改信息窗口的槽函数
void CKernel::SLOT_ShowAlterInfo(){
    changeDialog->show();
}

//显示创建房间窗口的槽函数
void CKernel::SLOT_ShowCreateRoom(){
    createDialog->show();
}

//显示添加好友窗口的槽函数
void CKernel::SLOT_ShowAddFriend(){
    addDialog->show();
}

//显示查找房间
void CKernel::SLOT_ShowJoinRoom(){
    joinDialog->show();
}

//刷新房间列表
void CKernel::SLOT_ReGetRoomTable(){
    auto ite = vec_roomlist.begin();
    while(ite!=vec_roomlist.end()){
        m_MainScene->Slot_RemoveUserItem(*ite);
        delete *ite;
        *ite = NULL;
        ite++;
    }
    m_MainScene->repaint();
    STRU_ASKROOM_RQ rq;
    m_tcpClient->SendData((char*)&rq,sizeof(rq));
}

//获取好友列表
void CKernel::SLOT_GetFriendList(){
    //发送查询好友列表包
    STRU_GETFRILIST_RQ rq;
    rq.m_userid = this->m_id;
    m_tcpClient->SendData((char*)&rq,sizeof(rq));
}
