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
#include"cardbutton.h"
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
    usecardtoid1 = 0;//出牌对象1
    usecardtoid2 = 0;//出牌对象2
    b_flagpush = true;
    chupai = new MyPushButton(":/res/icon/chupai.png",":/res/icon/chupai_1.png");


    qipai = new MyPushButton(":/res/icon/qipai.png",":/res/icon/qipai_1.png");

    b_choosefirstpeople = false;
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
    NetPackMap(DEF_PACK_ROOM_MEMBER_RS) = &CKernel::SLOT_DealRoomMemberRs;
    NetPackMap(DEF_PACK_TURN_BEGIN) = &CKernel::SLOT_DealTurnBeginRs;
    NetPackMap(DEF_PACK_POSTCARD_RS) = &CKernel::SLOT_DealPostCardRs;
    NetPackMap(DEF_PACK_POSTCARD_RQ) = &CKernel::SLOT_DealReposeCardRq;
}

//同步出牌动画
void CKernel::SLOT_DealReposeCardRq(char *buf,int nlen){
    STRU_POSTCARD_RQ *rq = (STRU_POSTCARD_RQ *)buf;
    QString checkname = GetCardName(rq->m_card.id);
    //QString resposename = GetCardName(rq);
    auto ite = this->m_mapSeatIdToId.begin();
    while(ite != this->m_mapSeatIdToId.end()){
        if((*ite).second == rq->m_userid){
            QString path;
            path = GetCardPath(rq->m_card.id);
            CardButton *card = new CardButton(path,"");
            card->num = rq->m_card.num;
            card->id = rq->m_card.id;
            card->color = rq->m_card.col;
            card->type = rq->m_card.type;
            card->setParent(this->gamingdlg);
            card->move(this->m_mapSeatIdToPosition[(*ite).first][0]+290,this->m_mapSeatIdToPosition[(*ite).first][1]);
            card->show();
            gamingdlg->update();
            card->PushCard();
            break;
        }
        ++ite;
    }
    if(rq->m_touser1id == this->m_id){
        label = new QLabel;
        label->setText(QString("您需要出一张%1").arg(checkname));
        QFont font("华文行楷",20,QFont::Bold);
        label->setFont(font);
        label->setStyleSheet("color:white;");
        label->setGeometry(800,800,250,250);
        label->setParent(gamingdlg);
        label->show();
        gamingdlg->update();
        chupai->show();
        qipai->show();
        b_flagpush = false;//被动出牌
    }

    //出牌按钮的槽函数
    connect(chupai,&MyPushButton::clicked,[=](){
        auto ite = this->vec_card.begin();
        while(ite!=this->vec_card.end()){
            if((*ite)->b_flagchoose){
                pushCard = *ite;
                (*ite)->PushCard();
                (*ite)->setEnabled(true);
                //(*ite)->hide();
                ite = this->vec_card.erase(ite);
                this->cardnum--;
            }else{
                ++ite;
            }
        }
        if(this->b_flagpush){
            STRU_POSTCARD_RQ rq1;
            rq1.m_roomid = this->m_roomid;
            rq1.m_userid = this->m_id;
            rq1.m_card.col = this->choosecard.col;
            rq1.m_card.id = this->choosecard.id;
            rq1.m_card.num = this->choosecard.num;
            rq1.m_card.type = this->choosecard.type;
            rq1.m_touser1id = usecardtoid1;
            rq1.m_touser2id = usecardtoid2;
            rq1.last_cardnum = this->cardnum;
            this->b_choosefirstpeople = false;
            m_tcpClient->SendData((char*)&rq1,sizeof(rq1));
        }else{
            STRU_POSTCARD_RS_S rs;
            rs.m_card.col = this->choosecard.col;
            rs.m_card.id = this->choosecard.id;
            rs.m_card.num = this->choosecard.num;
            rs.m_card.type = this->choosecard.type;
            if(rq->m_card.id == SHA){
                if(this->choosecard.id != SHAN){
                    return;
                }
            }
            rs.room_id = this->m_roomid;
            rs.user_id = this->m_id;
            rs.y_card = rq->m_card;
            rs.y_user_id = rq->m_userid;
            rs.m_lResult = 1;
            m_tcpClient->SendData((char*)&rs,sizeof(rs));
        }

    });
}

QString CKernel::GetCardPath(int cardid){
    QString path;
    switch(cardid){
    case SHA:
        path = QString(":/res/PAI/杀.png");
        break;
    case SHAN:
        path = QString(":/res/PAI/闪.png");
        break;
    case TAO:
        path = QString(":/res/PAI/桃.png");
        break;
    case GUOHECHAIQIAO:
        path = QString(":/res/PAI/过河拆桥.png");
        break;
    case SHUNSHOUQIANYANG:
        path = QString(":/res/PAI/顺手牵羊.png");
        break;
    case JUEDOU:
        path = QString(":/res/PAI/决斗.png");
        break;
    case JIEDAOSHAREN:
        path = QString(":/res/PAI/借刀杀人.png");
        break;
    case WUZHONGSHENGYOU:
        path = QString(":/res/PAI/无中生有.png");
        break;
    case WUXIEKEJI:
        path = QString(":/res/PAI/无懈可击.png");
        break;
    case WANJIANQIFA:
        path = QString(":/res/PAI/万箭齐发.png");
        break;
    case NANMANRUQIN:
        path = QString(":/res/PAI/南蛮入侵.png");
        break;
    case TAOYUANJIEYI:
        path = QString(":/res/PAI/桃园结义.png");
        break;
    case WUGUFENGDENG:
        path = QString(":/res/PAI/五谷丰登.png");
        break;
    case SHANDIAN:
        path = QString(":/res/PAI/闪电.png");
        break;
    case LEBUSISHU:
        path = QString(":/res/PAI/乐不思蜀.png");
        break;
    case HANBINGJIAN:
        path = QString(":/res/WUQI/寒冰剑.png");
        break;
    case CIXIONGSHUANGGUJIAN:
        path = QString(":/res/WUQI/雌雄双剑.png");
        break;
    case QINGLONGYANYUEDAO:
        path = QString(":/res/WUQI/青龙偃月刀.png");
        break;
    case QINGGANGJIAN:
        path = QString(":/res/WUQI/青钢剑.png");
        break;
    case ZHANGBASHEMAO:
        path = QString(":/res/WUQI/丈八蛇矛.png");
        break;
    case QILINGONG:
        path = QString(":/res/WUQI/麒麟弓.png");
        break;
    case ZHUGELIANNU:
        path = QString(":/res/WUQI/诸葛连弩.png");
        break;
    case GUANSHIFU:
        path = QString(":/res/WUQI/贯石斧.png");
        break;
    case FANGTIANHUAJI:
        path = QString(":/res/WUQI/方天画戟.png");
        break;
    case BAGUAZHEN:
        path = QString(":/res/WUQI/八卦阵.png");
        break;
    case RENWANGDUN:
        path = QString(":/res/WUQI/仁王盾.png");
        break;
    case CHITU:
        path = QString(":/res/MA/赤兔.png");
        break;
    case DAYUAN:
        path = QString(":/res/MA/大宛.png");
        break;
    case DILU:
        path = QString(":/res/MA/的卢.png");
        break;
    case JUEYING:
        path = QString(":/res/MA/绝影.png");
        break;
    case ZHUAHUANGFEIDIAN:
        path = QString(":/res/MA/爪黄飞电.png");
        break;
    case ZIXIN:
        path = QString(":/res/MA/紫骍.png");
        break;
    }
    return path;
}

QString CKernel::GetCardName(int cardid){
    QString result;
    switch(cardid){
    case SHA:
        result = QString("杀");
        break;
    case SHAN:
        result = QString("闪");
        break;
    case TAO:
        result = QString("桃");
        break;
    case GUOHECHAIQIAO:
        result = QString("过河拆桥");
        break;
    case SHUNSHOUQIANYANG:
        result = QString("顺手牵羊");
        break;
    case JUEDOU:
        result = QString("决斗");
        break;
    case JIEDAOSHAREN:
        result = QString("借刀杀人");
        break;
    case WUZHONGSHENGYOU:
        result = QString("无中生有");
        break;
    case WUXIEKEJI:
        result = QString("无懈可击");
        break;
    case WANJIANQIFA:
        result = QString("万箭齐发");
        break;
    case NANMANRUQIN:
        result = QString("南蛮入侵");
        break;
    case TAOYUANJIEYI:
        result = QString("桃园结义");
        break;
    case WUGUFENGDENG:
        result = QString("五谷丰登");
        break;
    case SHANDIAN:
        result = QString("闪电");
        break;
    case LEBUSISHU:
        result = QString("乐不思蜀");
        break;
    case HANBINGJIAN:
        result = QString("寒冰箭");
        break;
    case CIXIONGSHUANGGUJIAN:
        result = QString("雌雄双股剑");
        break;
    case QINGLONGYANYUEDAO:
        result = QString("青龙偃月刀");
        break;
    case QINGGANGJIAN:
        result = QString("青钢剑");
        break;
    case ZHANGBASHEMAO:
        result = QString("丈八蛇矛");
        break;
    case QILINGONG:
        result = QString("麒麟弓");
        break;
    case ZHUGELIANNU:
        result = QString("诸葛连弩");
        break;
    case GUANSHIFU:
        result = QString("贯石斧");
        break;
    case FANGTIANHUAJI:
        result = QString("方天画戟");
        break;
    case BAGUAZHEN:
        result = QString("八卦阵");
        break;
    case RENWANGDUN:
        result = QString("仁王盾");
        break;
    case CHITU:
        result = QString("赤兔");
        break;
    case DAYUAN:
        result = QString("大宛");
        break;
    case DILU:
        result = QString("的卢");
        break;
    case JUEYING:
        result = QString("绝影");
        break;
    case ZHUAHUANGFEIDIAN:
        result = QString("爪黄飞电");
        break;
    case ZIXIN:
        result = QString("紫骍");
        break;
    }
    return result;
}

//DEF_PACK_RESPOSE_CARD_RQ
//DEF_PACK_RESPOSE_CARD_RS
void CKernel::SLOT_DealPostCardRs(char *buf,int nlen){
    STRU_POSTCARD_RS *rs = (STRU_POSTCARD_RS *)buf;

}

//处理回合开始
void CKernel::SLOT_DealTurnBeginRs(char *buf,int nlen){
    //发送请求抽卡包
    STRU_TURN_BEGIN *rs = (STRU_TURN_BEGIN *)buf;
    if(rs->user_id == this->m_id){
        chupai->move(gamingdlg->width()*0.5-chupai->width()*0.5+40,gamingdlg->height()*0.8-150);
        chupai->setParent(gamingdlg);
        chupai->show();
        qipai->move(gamingdlg->width()*0.5-qipai->width()*0.5+200,gamingdlg->height()*0.8-150);
        qipai->setParent(gamingdlg);
        qipai->show();
        STRU_GETCARD_RQ rq;
        rq.m_roomid = this->m_roomid;
        rq.m_userid = this->m_id;
        rq.num = 2;
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
    }else{
        chupai->hide();
        qipai->hide();
    }

    //设置头像框
    headerborder = new MyPushButton(":/res/icon/头像框.png","");
    headerborder->setParent(gamingdlg);
    headerborder->move(gamingdlg->width()*0.5-myhero->width()*0.5-445,gamingdlg->height()*0.8-20);
    headerborder->show();
    gamingdlg->update();
}


//更新房间成员回复
void CKernel::SLOT_DealRoomMemberRs(char *buf,int nlen){
    STRU_ROOM_MEMBER_RS *rs = (STRU_ROOM_MEMBER_RS *)buf;
    for(int i=0;i<sizeof(rs->m_userInfo)/sizeof(rs->m_userInfo[0]);i++){
        if(rs->m_userInfo[i].m_place!=-1 && rs->m_userInfo[i].m_userid!=0){
            int k = rs->m_userInfo[i].m_place+1;
            m_mapSeatIdToId[k] = rs->m_userInfo[i].m_userid;
        }
    }

}

//处理抽卡回复
void CKernel::SLOT_DealGetCardRs(char *buf,int nlen){
    STRU_GETCARD_RS* rs = (STRU_GETCARD_RS*)buf;
    chupai->move(gamingdlg->width()*0.5-chupai->width()*0.5+40,gamingdlg->height()*0.8-150);
    chupai->setParent(this->gamingdlg);
    qipai->move(gamingdlg->width()*0.5-qipai->width()*0.5+200,gamingdlg->height()*0.8-150);
    qipai->setParent(this->gamingdlg);
    chupai->show();
    qipai->show();
    gamingdlg->update();
    for(int i=0;i<sizeof(rs->m_card)/sizeof(rs->m_card[0]);i++){
        if(rs->m_card[i].id != 0){
            CardButton *cardbtn;
            card.push_back(rs->m_card[i].id);
            switch(rs->m_card[i].id){
            case SHA:
                cardbtn = new CardButton(":/res/PAI/杀.png","");
                break;
            case SHAN:
                cardbtn = new CardButton(":/res/PAI/闪.png","");
                break;
            case TAO:
                cardbtn = new CardButton(":/res/PAI/桃.png","");
                break;
            case GUOHECHAIQIAO:
                cardbtn = new CardButton(":/res/PAI/过河拆桥.png","");
                break;
            case SHUNSHOUQIANYANG:
                cardbtn = new CardButton(":/res/PAI/顺手牵羊.png","");
                break;
            case JUEDOU:
                cardbtn = new CardButton(":/res/PAI/决斗.png","");
                break;
            case JIEDAOSHAREN:
                cardbtn = new CardButton(":/res/PAI/借刀杀人.png","");
                break;
            case WUZHONGSHENGYOU:
                cardbtn = new CardButton(":/res/PAI/无中生有.png","");
                break;
            case WUXIEKEJI:
                cardbtn = new CardButton(":/res/PAI/无懈可击.png","");
                break;
            case WANJIANQIFA:
                cardbtn = new CardButton(":/res/PAI/万箭齐发.png","");
                break;
            case NANMANRUQIN:
                cardbtn = new CardButton(":/res/PAI/南蛮入侵.png","");
                break;
            case TAOYUANJIEYI:
                cardbtn = new CardButton(":/res/PAI/桃园结义.png","");
                break;
            case WUGUFENGDENG:
                cardbtn = new CardButton(":/res/PAI/五谷丰登.png","");
                break;
            case SHANDIAN:
                cardbtn = new CardButton(":/res/PAI/闪电.png","");
                break;
            case LEBUSISHU:
                cardbtn = new CardButton(":/res/PAI/乐不思蜀.png"),"";
                break;
            case HANBINGJIAN:
                cardbtn = new CardButton(":/res/WUQI/寒冰剑.png","");
                break;
            case CIXIONGSHUANGGUJIAN:
                cardbtn = new CardButton(":/res/WUQI/雌雄双剑.png","");
                break;
            case QINGLONGYANYUEDAO:
                cardbtn = new CardButton(":/res/WUQI/青龙偃月刀.png","");
                break;
            case QINGGANGJIAN:
                cardbtn = new CardButton(":/res/WUQI/青钢剑.png","");
                break;
            case ZHANGBASHEMAO:
                cardbtn = new CardButton(":/res/WUQI/丈八蛇矛.png","");
                break;
            case QILINGONG:
                cardbtn = new CardButton(":/res/WUQI/麒麟弓.png","");
                break;
            case ZHUGELIANNU:
                cardbtn = new CardButton(":/res/WUQI/诸葛连弩.png","");
                break;
            case GUANSHIFU:
                cardbtn = new CardButton(":/res/WUQI/贯石斧.png","");
                break;
            case FANGTIANHUAJI:
                cardbtn = new CardButton(":/res/WUQI/方天画戟.png","");
                break;
            case BAGUAZHEN:
                cardbtn = new CardButton(":/res/WUQI/八卦阵.png","");
                break;
            case RENWANGDUN:
                cardbtn = new CardButton(":/res/WUQI/仁王盾.png","");
                break;
            case CHITU:
                cardbtn = new CardButton(":/res/MA/赤兔.png","");
                break;
            case DAYUAN:
                cardbtn = new CardButton(":/res/MA/大宛.png","");
                break;
            case DILU:
                cardbtn = new CardButton(":/res/MA/的卢.png","");
                break;
            case JUEYING:
                cardbtn = new CardButton(":/res/MA/绝影.png","");
                break;
            case ZHUAHUANGFEIDIAN:
                cardbtn = new CardButton(":/res/MA/爪黄飞电.png","");
                break;
            case ZIXIN:
                cardbtn = new CardButton(":/res/MA/紫骍.png","");
                break;
            }

            cardbtn->id = rs->m_card[i].id;
            cardbtn->num = rs->m_card[i].num;
            cardbtn->color = rs->m_card[i].col;
            cardbtn->type = rs->m_card[i].type;
            connect(cardbtn,&CardButton::clicked,[=](){
                if(cardbtn->b_flagchoose){
                    cardbtn->ChooseHero1();//向下
                }else{
                    cardbtn->ChooseHero();//向上
                }
                cardbtn->b_flagchoose = !cardbtn->b_flagchoose;
                choosecard.id = cardbtn->id;
                choosecard.num = cardbtn->num;
                choosecard.col = cardbtn->color;
                choosecard.type = cardbtn->type;
            });
            vec_card.push_back(cardbtn);
        }
    }
    this->cardnum = vec_card.size();
    InitCard();
}

void CKernel::InitCard(){
    auto ite = vec_card.begin();
    QString colorpath;
    QString numpath;
    int i=0;
    while(ite!=vec_card.end()){
        colorpath = GetColorPath((*ite)->color);
        numpath = GetNumPath((*ite)->num);
        MyPushButton *color = new MyPushButton(colorpath,"");
        color->setParent(*ite);
        color->move(12,11);
        MyPushButton *num = new MyPushButton(numpath,"");
        num->setParent(*ite);
        num->move(11,30);
        color->show();
        num->show();
        (*ite)->setParent(gamingdlg);
        (*ite)->move(gamingdlg->width()*0.5-160+i*140,gamingdlg->height()*0.8-10);
        (*ite)->show();
        gamingdlg->update();
        ++i;
        ++ite;
    }
}

QString CKernel::GetColorPath(int colorid){
    QString colorpath;
    switch(colorid){
        case Spade1:
            colorpath = QString(":/res/icon/heitao.png");
        break;
        case Hearts2:
            colorpath = QString(":/res/icon/hongtao.png");
        break;
        case Club3:
            colorpath = QString(":/res/icon/meihua.png");
        break;
        case Diamond4:
            colorpath = QString(":/res/icon/fangkuai.png");
        break;
    }
    return colorpath;
}

QString CKernel::GetNumPath(int num){
    QString numpath;
    switch(num){
        case 1:
            numpath = QString(":/res/Num/Num_A.png");
        break;
        case 2:
            numpath = QString(":/res/Num/num_2.png");
        break;
        case 3:
            numpath = QString(":/res/Num/Num_3.png");
        break;
        case 4:
            numpath = QString(":/res/Num/Num_4.png");
        break;
        case 5:
            numpath = QString(":/res/Num/Num_5.png");
        break;
        case 6:
            numpath = QString(":/res/Num/Num_6.png");
        break;
        case 7:
            numpath = QString(":/res/Num/Num_7.png");
        break;
        case 8:
            numpath = QString(":/res/Num/Num_8.png");
        break;
        case 9:
            numpath = QString(":/res/Num/Num_9.png");
        break;
        case 10:
            numpath = QString(":/res/Num/Num_10.png");
        break;
        case 11:
            numpath = QString(":/res/Num/Num_J.png");
        break;
        case 12:
            numpath = QString(":/res/Num/Num_Q.png");
        break;
        case 13:
            numpath = QString(":/res/Num/Num_K.png");
        break;
    }
    return numpath;
}

//处理返回所有人选择的英雄和自身用户id
void CKernel::SLOT_DealAllSelHeroRs(char *buf,int nlen){
    STRU_ALLSEL_HERO_RS *rs = (STRU_ALLSEL_HERO_RS *)buf;
    for(int i=0;i<sizeof(rs->m_playerarr)/sizeof(rs->m_playerarr[0]);i++){
        int seatid = FindSeatIdById(rs->m_playerarr[i].user_id);
        this->m_mapSeatIdToHeroId[seatid] = rs->m_playerarr[i].hero_id;
        this->m_mapSeatIdToHp[seatid] = rs->m_playerarr[i].hp;
    }
    ShowHero();
    ShowHp();
}

//显示所有人的血量
void CKernel::ShowHp(){
    auto ite = this->m_mapSeatIdToHp.begin();
    QString path;
    while(ite!= this->m_mapSeatIdToHp.end()){
        switch((*ite).second){
        case 1:
            path = QString(":/res/blood/一血.png");
            break;
        case 2:
            path = QString(":/res/blood/二血.png");
            break;
        case 3:
            path = QString(":/res/blood/三血.png");
            break;
        case 4:
            path = QString(":/res/blood/四血.png");
            break;
        case 5:
            path = QString(":/res/blood/五血.png");
                break;
        }

        MyPushButton *blood = new MyPushButton(path,"");
        blood->setParent(gamingdlg);
        if(this->MySeatId == (*ite).first){
            blood->move(gamingdlg->width()*0.5-blood->width()*0.5-300,gamingdlg->height()*0.8-10);
        }else{
            blood->move(this->m_mapSeatIdToPosition[(*ite).first][0]+300,this->m_mapSeatIdToPosition[(*ite).first][1]);
        }
        blood->show();
        ++ite;
    }
    gamingdlg->update();
}

//显示所有人选择的英雄
void CKernel::ShowHero(){
    auto ite = this->m_mapSeatIdToHeroId.begin();
    while(ite!= this->m_mapSeatIdToHeroId.end()){
        if((*ite).first != this->MySeatId && (*ite).second != this->ZG_Id){
            QString path = GetHeroPath((*ite).second);
            HeroButton *hero = new HeroButton(path,"");
            hero->seatid = (*ite).first;
            hero->setParent(gamingdlg);
            hero->move(this->m_mapSeatIdToPosition[(*ite).first][0]+140,this->m_mapSeatIdToPosition[(*ite).first][1]);
            hero->show();
            connect(hero,&HeroButton::clicked,[=](){
                if(!this->b_choosefirstpeople){
                    //如果没选择第一个人
                    this->usecardtoid1 = this->m_mapSeatIdToId[hero->seatid];
                    this->b_choosefirstpeople = true;
                }else{
                    //如果选择了第一个人
                    this->usecardtoid2 = this->m_mapSeatIdToId[hero->seatid];
                }
            });
            gamingdlg->update();
        }
        ++ite;
    }
}

int CKernel::FindSeatIdById(int myid){
    auto ite = this->m_mapSeatIdToId.begin();
    while(ite != this->m_mapSeatIdToId.end()){
        if((*ite).second == myid){
            return (*ite).first;
        }
        ++ite;
    }
    return 0;
}

//处理选择英雄
void CKernel::SLOT_DealSelectHero(char *buf,int nlen){

    STRU_SELHERO_RQ *rq = (STRU_SELHERO_RQ*)buf;
    if(this->m_identity != zhugong){
        this->ZG_HeroId = rq->ZG_heroid;
        //显示主公
        //通过主公id找主公座位号
        int SeatIdofZG = FindSeatIdById(this->ZG_Id);
        //显示主公英雄
        QString path = GetHeroPath(ZG_HeroId);
        HeroButton *hero = new HeroButton(path,"");
        hero->seatid = SeatIdofZG;
        hero->setParent(gamingdlg);
        hero->move(this->m_mapSeatIdToPosition[SeatIdofZG][0]+140,this->m_mapSeatIdToPosition[SeatIdofZG][1]);
        hero->show();
        gamingdlg->update();
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
                hero->ChooseHero1();//向下
            }else{
                auto ite = vec_hero.begin();
                while(ite!=vec_hero.end()){
                    if((*ite)!=hero){
                        if((*ite)->b_flagchoose){
                            (*ite)->ChooseHero1();
                            (*ite)->b_flagchoose = !(*ite)->b_flagchoose;
                        }
                    }
                    ++ite;
                }
                hero->ChooseHero();//向上
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
        rs.iddentity = this->m_identity;
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
        myhero->move(gamingdlg->width()*0.5-myhero->width()*0.5-440,gamingdlg->height()*0.8-10);
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
        identity->move(gamingdlg->width()*0.5-identity->width()*0.5-600,gamingdlg->height()*0.8-10);
        identity->show();
        gamingdlg->update();

        QTimer::singleShot(2000,this,[=]{
            identityattention->setParent(gamingdlg);
            identityattention->move(gamingdlg->width()*0.5-identityattention->width()*0.5,gamingdlg->height()*0.5);
            identityattention->show();
            gamingdlg->update();
        });

        QTimer::singleShot(4000,this,[=]{
            identityattention->hide();
            delete identityattention;
            gamingdlg->update();
        });
        //显示主公位置
        ShowZgPosition();
}

void CKernel::ShowZgPosition(){
    int myposition = 0;
    auto ite = m_mapSeatIdToId.begin();
    int size = m_mapSeatIdToId.size();
    int index = 0;
    int mark = 1;
    while(ite != m_mapSeatIdToId.end()){
        if((*ite).first == this->MySeatId){
            myposition = index;
            break;
        }
        ++ite;
        ++index;
    }
    ++ite;
    ++index;

    for(int i=index;i<size;i++){
        ShowZGIdentity(mark,(*ite).second,(*ite).first);
        ++mark;
        ++ite;
    }
    auto ite1 = m_mapSeatIdToId.begin();
    int temp = mark;
    if(mark<5){
        for(int j=0;j<size-temp;j++){
            ShowZGIdentity(mark,(*ite1).second,(*ite1).first);
            ++mark;
            ++ite1;
        }
    }
}

void CKernel::ShowZGIdentity(int mark,int id,int seatid){
    QString path;
    MyPushButton *identitybutton;
    if(id == this->ZG_Id){
        path = QString(":/res/Shenfen/主公.png");
    }else{
        path = QString(":/res/Shenfen/身份背面.jpg");
    }
    if(mark == 1){
        identitybutton = new MyPushButton(path,"");
        identitybutton->setParent(gamingdlg);
        identitybutton->move(30,gamingdlg->height()*0.5-95);
        identitybutton->show();
        gamingdlg->update();
        vector<int>positon;
        positon.push_back(30);
        positon.push_back(gamingdlg->height()*0.5-95);
        m_mapSeatIdToPosition[seatid] = positon;
    }else if(mark == 2){
        identitybutton = new MyPushButton(path,"");
        identitybutton->setParent(this->gamingdlg);
        identitybutton->move(500,30);
        identitybutton->show();
        gamingdlg->update();
        vector<int>positon;
        positon.push_back(500);
        positon.push_back(30);
        m_mapSeatIdToPosition[seatid] = positon;
    }else if(mark == 3){
        identitybutton = new MyPushButton(path,"");
        identitybutton->setParent(this->gamingdlg);
        identitybutton->move(gamingdlg->width()-640,30);
        identitybutton->show();
        gamingdlg->update();
        vector<int>positon;
        positon.push_back(gamingdlg->width()-640);
        positon.push_back(30);
        m_mapSeatIdToPosition[seatid] = positon;
    }else{
        identitybutton = new MyPushButton(path,"");
        identitybutton->setParent(gamingdlg);
        identitybutton->move(gamingdlg->width()-500,gamingdlg->height()*0.5-95);
        identitybutton->show();
        gamingdlg->update();
        vector<int>positon;
        positon.push_back(gamingdlg->width()-500);
        positon.push_back(gamingdlg->height()*0.5-95);
        m_mapSeatIdToPosition[seatid] = positon;
    }

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
                this->MySeatId = rs->place+1;
                int k = rs->place+1;
                this->m_mapSeatIdToId[k] = this->m_id;
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
            this->m_mapSeatIdToId[1] = this->m_id;
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
