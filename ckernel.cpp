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
#include"showothercard.h"
#define MD5_KEY "1234"
#include"QCryptographicHash"
#define NetPackMap(a) m_NetPackMap[(a)-DEF_PACK_BASE]
static QByteArray GetMD5( QString val)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    QString tmp = QString( "%1_%2").arg(val).arg(MD5_KEY);

    hash.addData( tmp.toStdString().c_str() , strlen(tmp.toStdString().c_str()) );
    QByteArray  result = hash.result();

    return result.toHex();
}
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
    showothercarddlg = new ShowOtherCard;
    friendlistDialog = new FriendList;
    item = new RoomItem;
    killaction = new MyPushButton(":/res/icon/杀指向.png","");
    chooseid = -1;
    m_roomcount = 0;
    usecardtoid1 = 0;//出牌对象1
    usecardtoid2 = 0;//出牌对象2
    b_flagpush = true;
    b_isKill = false;//默认没出过杀
    ChuPai = new MyPushButton(":/res/icon/chupai.png",":/res/icon/chupai_1.png");
    BuChu = new MyPushButton(":/res/icon/buchu.png",":/res/icon/buchu_1.png");
    ChuPai->hide();
    BuChu->hide();
    chupai = new MyPushButton(":/res/icon/chupai.png",":/res/icon/chupai_1.png");
    qipai = new MyPushButton(":/res/icon/qipai.png",":/res/icon/qipai_1.png");
    chupai->setParent(gamingdlg);
    qipai->setParent(gamingdlg);
    chupai->hide();
    qipai->hide();
    queding = new MyPushButton(":/res/icon/queding.png",":/res/icon/queding_1.png");
    queren = new MyPushButton(":/res/icon/queding.png",":/res/icon/queding_1.png");
    turnlogo = new MyPushButton(":/res/icon/turnlogo.png","");
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
        QByteArray array = GetMD5(password);
        memcpy(rq.m_szPassword,array.data(),array.size());
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

    connect(chupai,&MyPushButton::clicked,this,&CKernel::SLOT_CHUPAI);

    connect(queding,&MyPushButton::clicked,this,&CKernel::SLOT_QUEDING);
    //弃牌槽函数
    connect(qipai,&MyPushButton::clicked,[=](){
        queding->setParent(gamingdlg);
        queding->move(gamingdlg->width()*0.5-queding->width()*0.5+200,gamingdlg->height()*0.8-150);
        queding->show();
        gamingdlg->update();
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

void CKernel::SLOT_QUEDING(){
    if(this->m_queQuitCard.size()>0){
        while (!m_queQuitCard.empty())
            m_queQuitCard.pop();
    }
    STRU_OFFCARD_RQ rq;
    if(this->vec_pushcard.size()>0){
        for(int i=0;i<vec_pushcard.size();i++){
            vec_pushcard[i]->hide();
            gamingdlg->update();
        }
    }
    if(this->vec_otherpushcard.size()>0){
        for(int i=0;i<vec_otherpushcard.size();i++){
            vec_otherpushcard[i]->hide();
            gamingdlg->update();
        }
    }
    //需要弃牌的数量
    int needquit = this->vec_card.size()-this->myhp;
    if(needquit>0){
        auto ite = this->vec_card.begin();
        while(ite!=this->vec_card.end()){
            if((*ite)->b_flagchoose){
                pushCard = *ite;
                //放入弃牌队列
                this->m_queQuitCard.push(*ite);
                //如果弃牌队列数量大于needquit
                //删除队首 并让队首的牌向下移动
                while(this->m_queQuitCard.size()>needquit){
                    CardButton *pMark = this->m_queQuitCard.front();
                    pMark->b_flagchoose = !pMark->b_flagchoose;
                    this->m_queQuitCard.pop();
                    pMark->zoom1();
                }
                //删除牌
                ++ite;
            }else{
                ++ite;
            }
        }
    }
    if(m_queQuitCard.size()<needquit){
        QMessageBox::about(gamingdlg,"提示","弃牌数量过少");
        return;
    }
    int i=0;
    while(!m_queQuitCard.empty()){

        CardButton *pPush = this->m_queQuitCard.front();
        vec_card.erase(remove(vec_card.begin(),vec_card.end(),pPush),vec_card.end());
        this->cardnum--;
        //弃牌数组赋值
        rq.m_offcard[i].col = pPush->color;
        rq.m_offcard[i].id = pPush->id;
        rq.m_offcard[i].num = pPush->num;
        rq.m_offcard[i].type = pPush->type;
        this->m_queQuitCard.pop();
        pPush->PushCard();
        pPush->setEnabled(true);
        this->vec_pushcard.push_back(pPush);
        gamingdlg->update();
        i++;
    }
    if(this->myhp<this->vec_card.size()){
        QMessageBox::about(gamingdlg,"提示","弃牌数量过少");
    }else{
        rq.m_user_id = this->m_id;
        rq.m_roomid = this->m_roomid;
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
        for(int i=0;i<this->vec_card.size();i++){
            vec_card[i]->hide();
        }
        InitCard();
        queding->hide();
        chupai->hide();
        qipai->hide();
    }
}

void CKernel::SLOT_CHUPAI(){
    auto ite = this->vec_card.begin();
    while(ite!=this->vec_card.end()){
        if((*ite)->b_flagchoose){
            if((*ite)->id != SHA || ((*ite)->id == SHA && this->b_isKill == false)){
                pushCard = *ite;
                if(vec_otherpushcard.size()>0){
                    for(int i=0;i<vec_otherpushcard.size();i++){
                        vec_otherpushcard[i]->hide();
                        gamingdlg->update();
                    }
                }

                if(this->vec_pushcard.size()>0){
                    for(int i=0;i<vec_pushcard.size();i++){
                        vec_pushcard[i]->hide();
                        gamingdlg->update();
                    }
                }
                (*ite)->PushCard();
                (*ite)->setEnabled(true);
                //显示攻击动画
                if(usecardtoid1!=0){
                    int seatid = FindSeatIdById(usecardtoid1);
                    killaction->setParent(gamingdlg);
                    killaction->move(gamingdlg->width()*0.5-killaction->width()*0.5-50,gamingdlg->height()*0.8-100);
                    killaction->show();
                    gamingdlg->update();
                    killaction->KillAction(this->m_mapSeatIdToPosition[seatid][0]+290,this->m_mapSeatIdToPosition[seatid][1]);
                    gamingdlg->update();
                    QTimer::singleShot(2000,this,[=]{
                        killaction->hide();
                        gamingdlg->update();
                    });
                }
                ite = this->vec_card.erase(ite);
                this->cardnum--;
                gamingdlg->update();

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
                if(rq1.m_card.id == SHA){
                    b_isKill = true;
                }
                for(int i=0;i<this->vec_card.size();i++){
                    vec_card[i]->hide();
                }
                InitCard();
                break;
            }
            return;
        }else{
            ++ite;
        }
    }

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
    NetPackMap(DEF_PACK_COMMIT_STATUS) = &CKernel::SLOT_CommitStatus;
    NetPackMap(DEF_PACK_OFFCARD_RQ) = &CKernel::SLOT_OffCardRq;
    NetPackMap(DEF_PACK_GHCQ_RQ) = &CKernel::SLOT_GHCQ_Rq;
    NetPackMap(DEF_PACK_GHCQ_RS) = &CKernel::SLOT_GHCQ_Rs;
    NetPackMap(DEF_PACK_SSQY_RQ) = &CKernel::SLOT_SSQY_Rq;
    NetPackMap(DEF_PACK_SSQY_RS) = &CKernel::SLOT_SSQY_Rs;
    NetPackMap(DEF_PACK_HEALPLAYER_RQ) = &CKernel::SLOT_HILIGHT_Rq;
    NetPackMap(DEF_PACK_HEALPLAYER_RS) = &CKernel::SLOT_HEALPLAYER_RQ;
    NetPackMap(DEF_PACK_PLAYER_DIE) = &CKernel::SLOT_PLAYERDIE;
}

void CKernel::SLOT_PLAYERDIE(char *buf,int nlen){
    STRU_PLAYER_DIE* rq = (STRU_PLAYER_DIE*)buf;
    int seatid = FindSeatIdById(rq->die_userid);
    MyPushButton *attention = new MyPushButton(":/res/icon/die.png",":/res/icon/die.png");
    attention->setParent(gamingdlg);
    attention->move(m_mapSeatIdToPosition[seatid][0],m_mapSeatIdToPosition[seatid][1]+50);
    attention->show();
    gamingdlg->update();
    MyPushButton *identity;
    switch(rq->iddentity){
        case zhugong:
        {
            identity = new MyPushButton(":/res/Shenfen/主公.png");
        }
        break;
        case zhongchen:
        {
            identity = new MyPushButton(":/res/Shenfen/忠臣.png");
        }
        break;
        case neijian:
        {
            identity = new MyPushButton(":/res/Shenfen/内奸.png");
        }
        break;
        case fanzei:
        {
            identity = new MyPushButton(":/res/Shenfen/反贼.png");
        }
        break;
    }
    identity->setParent(gamingdlg);
    identity->move(m_mapSeatIdToPosition[seatid][0],m_mapSeatIdToPosition[seatid][1]);
    identity->show();
    gamingdlg->update();
}

void CKernel::SLOT_HEALPLAYER_RQ(char *buf,int nlen){
    STRU_HEAL_PLAYER_RQ *rq = (STRU_HEAL_PLAYER_RQ *)buf;
    int seatid = FindSeatIdById(rq->die_userid);
    int dieuserid = rq->die_userid;
    bool flag = false;
    for(int i=0;i<vec_card.size();i++){
        if(vec_card[i]->id == TAO)
            flag = true;
    }
    if(flag){
        QLabel *label = new QLabel;
        QPalette label_pe;
        QFont ft;
        ft.setPointSize(20);
        label_pe.setColor(QPalette::WindowText, Qt::white);
        label->setPalette(label_pe);
        label->setFont(ft);
        label->setText(QString("是否使用桃?"));
        label->setParent(gamingdlg);
        label->setGeometry(700,700,400,400);
        label->show();
        QTimer::singleShot(3000,this,[=]{
            label->hide();
        });
        MyPushButton *cp = new MyPushButton(":/res/icon/chupai.png",":/res/icon/chupai_1.png");
        MyPushButton *bc = new MyPushButton(":/res/icon/buchu.png",":/res/icon/buchu_1.png");
        cp->setParent(this->gamingdlg);
        cp->move(gamingdlg->width()*0.5-ChuPai->width()*0.5+40,gamingdlg->height()*0.8-150);
        bc->setParent(this->gamingdlg);
        bc->move(gamingdlg->width()*0.5-BuChu->width()*0.5+200,gamingdlg->height()*0.8-150);
        cp->show();
        bc->show();
        gamingdlg->update();
        connect(cp,&MyPushButton::clicked,[=](){
            STRU_HEAL_PLAYER_RS rs;
            rs.die_userid = dieuserid;
            rs.n_lResult = post_success;
            rs.user_id = this->m_userid;
            rs.roomid = this->m_roomid;
            m_tcpClient->SendData((char*)&rs,sizeof(rs));
        });
        connect(bc,&MyPushButton::clicked,[=](){
            STRU_HEAL_PLAYER_RS rs;
            rs.die_userid = dieuserid;
            rs.n_lResult = post_failed;
            rs.user_id = this->m_userid;
            rs.roomid = this->m_roomid;
            m_tcpClient->SendData((char*)&rs,sizeof(rs));
        });
    }else{
        STRU_HEAL_PLAYER_RS rs;
        rs.die_userid = dieuserid;
        rs.n_lResult = post_failed;
        rs.user_id = this->m_userid;
        rs.roomid = this->m_roomid;
        m_tcpClient->SendData((char*)&rs,sizeof(rs));
    }
}


void CKernel::SLOT_HILIGHT_Rq(char *buf,int nlen){
    STRU_HILIGHT_RQ *rq = (STRU_HILIGHT_RQ *)buf;
    int seatid = FindSeatIdById(rq->m_userid);
    turnlogo->hide();

    turnlogo->setParent(gamingdlg);
    if(seatid == this->MySeatId){
        turnlogo->move(gamingdlg->width()*0.5-turnlogo->width()*0.5-445,gamingdlg->height()*0.8-55);
    }else{
        turnlogo->move(this->m_mapSeatIdToPosition[seatid][0]+55,this->m_mapSeatIdToPosition[seatid][1]);
    }
    turnlogo->show();
    gamingdlg->update();
}

//处理顺手牵羊回复
void CKernel::SLOT_SSQY_Rs(char *buf,int nlen){
    if(vec_otherpushcard.size()>0){
        for(int i=0;i<vec_otherpushcard.size();i++){
            vec_otherpushcard[i]->hide();
            gamingdlg->update();
        }
    }

    if(this->vec_pushcard.size()>0){
        for(int i=0;i<vec_pushcard.size();i++){
            vec_pushcard[i]->hide();
            gamingdlg->update();
        }
    }
    STRU_SSQY_RS *rs = (STRU_SSQY_RS *)buf;
    //如果顺手牵羊对自己使用
    if(rs->y_userid == this->m_id){
        auto ite = this->vec_card.begin();
        while(ite != this->vec_card.end()){
            if((*ite)->id == rs->m_card.id){
                (*ite)->PushCard();
                ite = this->vec_card.erase(ite);
                gamingdlg->update();
                this->cardnum--;
                vec_pushcard.push_back(*ite);
                InitCard();
                break;
            }
            ++ite;
        }
    }else{
        //被使用顺手牵羊的座位号
        int y_seatid = FindSeatIdById(rs->y_userid);
        //使用顺手牵羊的座位号
        int m_seatid = FindSeatIdById(rs->m_userid);
        QString path;
        path = GetCardPath(rs->m_card.id);
        CardButton *card = new CardButton(path,"");
        card->num = rs->m_card.num;
        card->id = rs->m_card.id;
        card->color = rs->m_card.col;
        card->type = rs->m_card.type;
        card->setParent(gamingdlg);
        card->move(this->m_mapSeatIdToPosition[y_seatid][0]+290,this->m_mapSeatIdToPosition[y_seatid][1]);
        card->show();
        card->CardAction(this->m_mapSeatIdToPosition[m_seatid][0]+290,this->m_mapSeatIdToPosition[m_seatid][1]);
        this->vec_otherpushcard.push_back(card);
        InitCard();

        QTimer::singleShot(2000,this,[=]{
            for(int i=0;i<vec_otherpushcard.size();i++){
                vec_otherpushcard[i]->hide();
                delete vec_otherpushcard[i];
                vec_otherpushcard[i] = NULL;
            }
        });
        gamingdlg->update();
    }
}

//处理顺手牵羊请求
void CKernel::SLOT_SSQY_Rq(char *buf,int nlen){
    STRU_SSQY_RQ *rq = (STRU_SSQY_RQ *)buf;
    yuserid = rq->y_userid;
    int count=0;
    vector<STRU_CARD>veccard;
    vector<CardButton*>vecCardButton;
    STRU_CARD fangju;
    STRU_CARD fangyuma;
    STRU_CARD jingongma;
    STRU_CARD wuqi;

    while(rq->m_card[count].id>0){
        veccard.push_back(rq->m_card[count]);
        count++;
    }
    if(rq->fj.id>0){
        fangju = rq->fj;
        veccard.push_back(fangju);

    }
    if(rq->fym.id>0){
        fangyuma = rq->fym;
        veccard.push_back(fangyuma);
    }
    if(rq->jgm.id>0){
        jingongma = rq->jgm;
        veccard.push_back(jingongma);
    }
    if(rq->wq.id>0){
        wuqi = rq->wq;
        veccard.push_back(wuqi);
    }
    //MyPushButton *queren = new MyPushButton(":/res/icon/queding.png",":/res/icon/queding_1.png");
    queren->setParent(showothercarddlg);
    queren->move(showothercarddlg->width()*0.5-queren->width()*0.5,showothercarddlg->height()-40);
    for(int i=0;i<veccard.size();i++){
        QString path;
        if(i<count){
            path = QString(":/res/PAI/背面.png");
        }else{
            path = GetCardPath(veccard[i].id);
        }
        CardButton *card = new CardButton(path);
        card->id = veccard[i].id;
        card->color = veccard[i].col;
        card->type = veccard[i].type;
        card->num = veccard[i].num;
        card->setParent(this->showothercarddlg);
        card->move(10+i*60,10);
        vecCardButton.push_back(card);
        connect(card,&CardButton::clicked,[=]()mutable {
            if(card->b_flagchoose){
                card->ChooseHero1();//向下
            }else{
                auto ite = vecCardButton.begin();
                while(ite != vecCardButton.end()){
                    if((*ite) != card){
                        if((*ite)->b_flagchoose){
                            (*ite)->ChooseHero1();
                            (*ite)->b_flagchoose = !(*ite)->b_flagchoose;
                        }
                    }
                    ++ite;
                }
                card->ChooseHero();//向上
                card->b_flagchoose = true;
            }
            choosecardofssqy.id = card->id;
            choosecardofssqy.type = card->type;
            choosecardofssqy.col = card->color;
            choosecardofssqy.num = card->num;
        });
}
    showothercarddlg->show();
    connect(this->queren,&MyPushButton::clicked,[=]()mutable{
        STRU_SSQY_RS rs;
        rs.m_card = choosecardofssqy;
        rs.m_userid = this->m_id;
        if(choosecardofssqy.type == WUQI){
            rs.n_lResult = wqpai;
        }else if(choosecardofssqy.type == FANGJU){
            rs.n_lResult = fjpai;
        }else if(choosecardofssqy.type == JINGONGMA){
            rs.n_lResult = jgmpai;
        }else if(choosecardofssqy.type == FANGYUMA){
            rs.n_lResult = fympai;
        }else{
            rs.n_lResult = shoupai;
        }
        rs.room_id = this->m_roomid;
        rs.y_userid = yuserid;
        m_tcpClient->SendData((char*)&rs,sizeof(rs));
        showothercarddlg->close();
        QString path = GetCardPath(rs.m_card.id);
        int seatid = FindSeatIdById(rs.y_userid);
        CardButton *card = new CardButton(path);
        CardButton *pMark = new CardButton(path);
        pMark->color = rs.m_card.col;
        pMark->num = rs.m_card.num;
        pMark->type = rs.m_card.type;
        pMark->id = rs.m_card.id;
        card->setParent(gamingdlg);
        card->move(this->m_mapSeatIdToPosition[seatid][0]+290,this->m_mapSeatIdToPosition[seatid][1]);
        card->show();
        card->CardAction(gamingdlg->width()*0.5-160+vec_card.size()*140,gamingdlg->height()*0.8-10);
        gamingdlg->update();
        this->vec_card.push_back(card);
        InitCard();
        gamingdlg->update();
    });
}

//处理过河拆桥回复
void CKernel::SLOT_GHCQ_Rs(char *buf,int nlen){
    if(vec_otherpushcard.size()>0){
        for(int i=0;i<vec_otherpushcard.size();i++){
            vec_otherpushcard[i]->hide();
            gamingdlg->update();
        }
    }

    if(this->vec_pushcard.size()>0){
        for(int i=0;i<vec_pushcard.size();i++){
            vec_pushcard[i]->hide();
            gamingdlg->update();
        }
    }
    STRU_GHCQ_RS *rs = (STRU_GHCQ_RS*)buf;
    if(rs->y_userid == this->m_id){
        auto ite = this->vec_card.begin();
        while(ite != this->vec_card.end()){
            if((*ite)->id == rs->m_card.id){
                CardButton *button = *ite;
                (*ite)->PushCard();
                ite = this->vec_card.erase(ite);
                gamingdlg->update();
                this->cardnum--;
                vec_pushcard.push_back(*ite);
                InitCard();
                break;
            }
            ++ite;
        }
    }else{
        int seatid = FindSeatIdById(rs->y_userid);
        QString path;
        path = GetCardPath(rs->m_card.id);
        CardButton *card = new CardButton(path,"");
        card->num = rs->m_card.num;
        card->id = rs->m_card.id;
        card->color = rs->m_card.col;
        card->type = rs->m_card.type;
        card->setParent(gamingdlg);
        card->move(this->m_mapSeatIdToPosition[seatid][0]+290,this->m_mapSeatIdToPosition[seatid][1]);
        card->show();
        card->PushCard();
        connect(card,&CardButton::clicked,[=](){
            if(card->b_flagchoose){
                card->ChooseHero1();//向下
            }else{
                card->ChooseHero();//向上
            }
            card->b_flagchoose = !card->b_flagchoose;
            choosecard.id = card->id;
            choosecard.num = card->num;
            choosecard.col = card->color;
            choosecard.type = card->type;
        });
        this->vec_otherpushcard.push_back(card);
        InitCard();
        gamingdlg->update();
    }
}

//处理过河拆桥请求
void CKernel::SLOT_GHCQ_Rq(char *buf,int nlen){
    STRU_GHCQ_RQ *rs = (STRU_GHCQ_RQ *)buf;
    yuserid = rs->y_userid;
    int count=0;
    vector<STRU_CARD>veccard;
    vector<CardButton*>vecCardButton;
    STRU_CARD fangju;
    STRU_CARD fangyuma;
    STRU_CARD jingongma;
    STRU_CARD wuqi;
    while(rs->m_card[count].id>0){
        veccard.push_back(rs->m_card[count]);
        count++;
    }
    if(rs->fj.id>0){
        fangju = rs->fj;
        veccard.push_back(fangju);

    }
    if(rs->fym.id>0){
        fangyuma = rs->fym;
        veccard.push_back(fangyuma);
    }
    if(rs->jgm.id>0){
        jingongma = rs->jgm;
        veccard.push_back(jingongma);
    }
    if(rs->wq.id>0){
        wuqi = rs->wq;
        veccard.push_back(wuqi);
    }

    queren->setParent(showothercarddlg);
    queren->move(showothercarddlg->width()*0.5-queren->width()*0.5,showothercarddlg->height()-40);
    for(int i=0;i<veccard.size();i++){
        QString path;
        if(i<count){
            path = QString(":/res/PAI/背面.png");
        }else{
            path = GetCardPath(veccard[i].id);
        }
        CardButton *card = new CardButton(path);
        card->id = veccard[i].id;
        card->color = veccard[i].col;
        card->type = veccard[i].type;
        card->num = veccard[i].num;
        card->setParent(this->showothercarddlg);
        card->move(10+i*60,10);
        vecCardButton.push_back(card);
        connect(card,&CardButton::clicked,[=]()mutable {
            if(card->b_flagchoose){
                card->ChooseHero1();//向下
            }else{
                auto ite = vecCardButton.begin();
                while(ite != vecCardButton.end()){
                    if((*ite) != card){
                        if((*ite)->b_flagchoose){
                            (*ite)->ChooseHero1();
                            (*ite)->b_flagchoose = !(*ite)->b_flagchoose;
                        }
                    }
                    ++ite;
                }
                card->ChooseHero();//向上
                card->b_flagchoose = true;
            }
            choosecardofghqc.id = card->id;
            choosecardofghqc.type = card->type;
            choosecardofghqc.col = card->color;
            choosecardofghqc.num = card->num;
        });
}
    showothercarddlg->show();
    connect(queren,&MyPushButton::clicked,[=]()mutable{
        STRU_GHCQ_RS rs;
        rs.m_card = choosecardofghqc;
        rs.m_userid = this->m_id;
        if(choosecardofghqc.type == WUQI){
            rs.n_lResult = wqpai;
        }else if(choosecardofghqc.type == FANGJU){
            rs.n_lResult = fjpai;
        }else if(choosecardofghqc.type == JINGONGMA){
            rs.n_lResult = jgmpai;
        }else if(choosecardofghqc.type == FANGYUMA){
            rs.n_lResult = fympai;
        }else{
            rs.n_lResult = shoupai;
        }
        rs.room_id = this->m_roomid;
        rs.y_userid = yuserid;
        m_tcpClient->SendData((char*)&rs,sizeof(rs));
        showothercarddlg->close();
        QString path = GetCardPath(rs.m_card.id);
        int seatid = FindSeatIdById(rs.y_userid);
        CardButton *card = new CardButton(path);
        card->setParent(gamingdlg);
        card->move(this->m_mapSeatIdToPosition[seatid][0]+290,this->m_mapSeatIdToPosition[seatid][1]);
        card->show();
        card->PushCard();
        gamingdlg->update();
        this->vec_otherpushcard.push_back(card);
    });
}

//同步弃牌动画
void CKernel::SLOT_OffCardRq(char *buf,int nlen){
    STRU_OFFCARD_RQ *rq = (STRU_OFFCARD_RQ *)buf;
    if(vec_otherpushcard.size()>0){
        for(int i=0;i<vec_otherpushcard.size();i++){
            vec_otherpushcard[i]->hide();
            gamingdlg->update();
        }
    }

    if(this->vec_pushcard.size()>0){
        for(int i=0;i<vec_pushcard.size();i++){
            vec_pushcard[i]->hide();
            gamingdlg->update();
        }
    }
    int seatid = FindSeatIdById(rq->m_user_id);
    int i=0;
    while(rq->m_offcard[i].id>0){
        QString path;
        path = GetCardPath(rq->m_offcard[i].id);
        CardButton *card = new CardButton(path,"");
        card->num = rq->m_offcard[i].num;
        card->id = rq->m_offcard[i].id;
        card->color = rq->m_offcard[i].col;
        card->type = rq->m_offcard[i].type;
        card->setParent(gamingdlg);
        card->move(this->m_mapSeatIdToPosition[seatid][0]+290,this->m_mapSeatIdToPosition[seatid][1]);
        card->show();
        i++;
        if(vec_otherpushcard.size()>0){
            for(int i=0;i<vec_otherpushcard.size();i++){
                vec_otherpushcard[i]->hide();
                gamingdlg->update();
            }
        }
        card->PushCard();
        this->vec_otherpushcard.push_back(card);
        gamingdlg->update();
    }
}

//如果有人掉血了 同步所有人的血量
void CKernel::SLOT_CommitStatus(char *buf,int nlen){
    STRU_COMMIT_STATUS *rs = (STRU_COMMIT_STATUS *)buf;
    int seatid = FindSeatIdById(rs->user_id);
    if(seatid == this->MySeatId){
        this->myhp = rs->hp_change;
    }
    //改变映射的值
    this->m_mapSeatIdToHp[seatid] = rs->hp_change;
    this->ShowHp();

}

//同步出牌动画
void CKernel::SLOT_DealReposeCardRq(char *buf,int nlen){
    STRU_POSTCARD_RQ *rq = (STRU_POSTCARD_RQ *)buf;
    killcard = rq->m_card;
    m_userid = rq->m_userid;
    y_userid = rq->m_touser1id;

    ChuPai->hide();
    BuChu->hide();
    QString checkname = GetCardName(rq->m_card.id);
    //QString resposename = GetCardName(rq);
    auto ite = this->m_mapSeatIdToId.begin();
    while(ite != this->m_mapSeatIdToId.end()){
        if((*ite).second == rq->m_userid){
            //找到出牌的那个人
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
            if(rq->m_touser1id!=0){
                if(rq->isShow){
                    if(rq->m_touser1id != this->m_id){
                        //显示攻击动画
                        int seatid = FindSeatIdById(rq->m_touser1id);
                        killaction->setParent(gamingdlg);
                        killaction->move(gamingdlg->width()*0.5-killaction->width()*0.5-50,gamingdlg->height()*0.8-100);
                        killaction->show();
                        gamingdlg->update();
                        killaction->KillAction(this->m_mapSeatIdToPosition[seatid][0]+290,this->m_mapSeatIdToPosition[seatid][1]-100);
                        QTimer::singleShot(2000,this,[=]{
                            killaction->hide();
                            gamingdlg->update();
                        });
                    }else{
                        //显示攻击动画
                        killaction->setParent(gamingdlg);
                        int seatid = FindSeatIdById(rq->m_userid);
                        killaction->move(this->m_mapSeatIdToPosition[seatid][0]+290,this->m_mapSeatIdToPosition[seatid][1]);
                        killaction->show();
                        gamingdlg->update();
                        killaction->KillAction(gamingdlg->width()*0.5-killaction->width()*0.5-50,gamingdlg->height()*0.8-100);
                        QTimer::singleShot(2000,this,[=]{
                            killaction->hide();
                            gamingdlg->update();
                        });
                    }
                }
            }

            gamingdlg->update();
            if(vec_otherpushcard.size()>0){
                for(int i=0;i<vec_otherpushcard.size();i++){
                    vec_otherpushcard[i]->hide();
                    gamingdlg->update();
                }
            }

            //if(vec_pushcard)
            card->PushCard();
            this->vec_otherpushcard.push_back(card);
            break;
        }
        ++ite;
    }

    //如果被使用牌的是自己 弹出一个QLabel 并执行动画
    if(rq->m_touser1id == this->m_id){

        if(rq->m_card.type == YANSHIJINNANG || rq->m_card.type == FEIYANSHIJINNANG){
            int flag = false;
            for(int i=0;i<vec_card.size();i++){
                if(vec_card[i]->id == WUXIEKEJI){
                    flag = true;
                }
            }
            if(flag == false){
                STRU_POSTCARD_RS_S rs;
                rs.m_lResult = post_failed;
                rs.room_id = this->m_roomid;
                rs.user_id = y_userid;
                rs.y_card = killcard;
                rs.y_user_id = m_userid;
                m_tcpClient->SendData((char*)&rs,sizeof(rs));
                return;
            }
        }
        //显示出牌弃牌按钮
        MyPushButton *cp = new MyPushButton(":/res/icon/chupai.png",":/res/icon/chupai_1.png");
        MyPushButton *bc = new MyPushButton(":/res/icon/buchu.png",":/res/icon/buchu_1.png");
        cp->setParent(this->gamingdlg);
        cp->move(gamingdlg->width()*0.5-ChuPai->width()*0.5+40,gamingdlg->height()*0.8-150);
        bc->setParent(this->gamingdlg);
        bc->move(gamingdlg->width()*0.5-BuChu->width()*0.5+200,gamingdlg->height()*0.8-150);
        cp->show();
        bc->show();
        gamingdlg->update();
        //出牌按钮的槽函数
        connect(cp,&MyPushButton::clicked,[=](){
                if(this->vec_pushcard.size()>0){
                    for(int i=0;i<vec_pushcard.size();i++){
                        vec_pushcard[i]->hide();
                        gamingdlg->update();
                    }
                }
                if(this->vec_otherpushcard.size()>0){
                    for(int i=0;i<vec_otherpushcard.size();i++){
                        vec_otherpushcard[i]->hide();
                        gamingdlg->update();
                    }
                }
                STRU_POSTCARD_RS_S rs;
                rs.m_lResult = post_success;
                rs.m_card.col = this->choosecard.col;
                rs.m_card.id = this->choosecard.id;
                rs.m_card.num = this->choosecard.num;
                rs.m_card.type = this->choosecard.type;
                if(rq->m_card.id == SHA){
                    if(this->choosecard.id != SHAN){
                        return;
                    }
                }
                if(rq->m_card.id == GUOHECHAIQIAO){
                    if(this->choosecard.id!=WUXIEKEJI){
                        return;
                    }
                }
                rs.room_id = this->m_roomid;
                rs.user_id = this->m_id;
                rs.y_card = killcard;
                rs.y_user_id = m_userid;
                rs.m_lResult = 1;
                m_tcpClient->SendData((char*)&rs,sizeof(rs));
                //将出的牌弹到桌面中间
                auto ite = this->vec_card.begin();
                while(ite!=this->vec_card.end()){
                    if((*ite)->b_flagchoose){
                        pushCard = *ite;
                        pushCard->PushCard();
                        pushCard->setEnabled(true);
                        InitCard();
                        this->vec_pushcard.push_back(pushCard);
                        ite = this->vec_card.erase(ite);
                        this->cardnum--;
                    }else{
                        ++ite;
                    }
                }
                for(int i=0;i<this->vec_card.size();i++){
                    vec_card[i]->hide();
                    gamingdlg->update();
                }
                for(int i=0;i<this->vec_otherpushcard.size();i++){
                    vec_otherpushcard[i]->hide();
                    gamingdlg->update();
                }
                InitCard();
                ChuPai->hide();
                BuChu->hide();
                gamingdlg->update();
        });

        connect(bc,&MyPushButton::clicked,[=]()mutable{
            STRU_POSTCARD_RS_S rs;
            rs.m_lResult = post_failed;
            rs.room_id = this->m_roomid;
            rs.user_id = this->m_id;
            rs.y_card = killcard;
            rs.y_user_id = m_userid;
            m_tcpClient->SendData((char*)&rs,sizeof(rs));
            bc->hide();
            cp->hide();
            gamingdlg->update();
        });
        gamingdlg->update();
        b_flagpush = false;//被动出牌
    }else{//如果不是对自己使用的牌 隐藏按钮
        if(rq->m_card.type == FEIYANSHIJINNANG || rq->m_card.type == YANSHIJINNANG){
            int flag = false;
            for(int i=0;i<vec_card.size();i++){
                if(vec_card[i]->id == WUXIEKEJI){
                    flag = true;
                }
            }
            if(flag){
                QLabel *label = new QLabel;
                QPalette label_pe;
                QFont ft;
                ft.setPointSize(20);
                label_pe.setColor(QPalette::WindowText, Qt::white);
                label->setPalette(label_pe);
                label->setFont(ft);
                label->setText(QString("是否使用无懈可击?"));
                label->setParent(gamingdlg);
                label->setGeometry(700,700,400,400);
                label->show();
                QTimer::singleShot(3000,this,[=]{
                    label->hide();
                });
                ChuPai->setParent(this->gamingdlg);
                ChuPai->move(gamingdlg->width()*0.5-ChuPai->width()*0.5+40,gamingdlg->height()*0.8-150);
                BuChu->setParent(this->gamingdlg);
                BuChu->move(gamingdlg->width()*0.5-BuChu->width()*0.5+200,gamingdlg->height()*0.8-150);
                ChuPai->show();
                BuChu->show();
                gamingdlg->update();
                connect(ChuPai,&MyPushButton::clicked,[=](){
                        if(this->vec_pushcard.size()>0){
                            for(int i=0;i<vec_pushcard.size();i++){
                                vec_pushcard[i]->hide();
                                gamingdlg->update();
                            }
                        }
                        if(this->vec_otherpushcard.size()>0){
                            for(int i=0;i<vec_otherpushcard.size();i++){
                                vec_otherpushcard[i]->hide();
                                gamingdlg->update();
                            }
                        }
                        STRU_POSTCARD_RS_S rs;
                        rs.m_lResult = post_success;
                        rs.m_card.col = this->choosecard.col;
                        rs.m_card.id = this->choosecard.id;
                        rs.m_card.num = this->choosecard.num;
                        rs.m_card.type = this->choosecard.type;
                        if(this->choosecard.id != WUXIEKEJI){
                            return;
                        }
                        rs.room_id = this->m_roomid;
                        rs.user_id = this->m_id;
                        rs.y_card = killcard;
                        rs.y_user_id = rq->m_userid;
                        rs.m_lResult = 1;
                        m_tcpClient->SendData((char*)&rs,sizeof(rs));
                        //将出的牌弹到桌面中间
                        auto ite = this->vec_card.begin();
                        while(ite!=this->vec_card.end()){
                            if((*ite)->b_flagchoose){
                                pushCard = *ite;
                                pushCard->PushCard();
                                pushCard->setEnabled(true);
                                InitCard();
                                this->vec_pushcard.push_back(pushCard);
                                ite = this->vec_card.erase(ite);
                                this->cardnum--;
                            }else{
                                ++ite;
                            }
                        }
                        for(int i=0;i<this->vec_card.size();i++){
                            vec_card[i]->hide();
                            gamingdlg->update();
                        }
                        for(int i=0;i<this->vec_otherpushcard.size();i++){
                            vec_otherpushcard[i]->hide();
                            gamingdlg->update();
                        }
                        InitCard();
                        ChuPai->hide();
                        BuChu->hide();
                        gamingdlg->update();
                });

                connect(BuChu,&MyPushButton::clicked,[=](){
                    STRU_POSTCARD_RS_S rs;
                    rs.m_lResult = post_failed;
                    rs.room_id = this->m_roomid;
                    rs.user_id = y_userid;
                    rs.y_card = killcard;
                    rs.y_user_id = m_userid;
                    m_tcpClient->SendData((char*)&rs,sizeof(rs));
                    ChuPai->hide();
                    BuChu->hide();
                    gamingdlg->update();
                    return;
                });
            }else{
                STRU_POSTCARD_RS_S rs;
                rs.m_lResult = post_failed;
                rs.room_id = this->m_roomid;
                rs.user_id = y_userid;
                rs.y_card = killcard;
                rs.y_user_id = m_userid;
                m_tcpClient->SendData((char*)&rs,sizeof(rs));
            }
        }else{
            chupai->hide();
            qipai->hide();
            gamingdlg->update();
        }

    }
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
    switch(rs->m_lResult){
        case WAIT_POST_CARD:
            chupai->hide();
            qipai->hide();
            gamingdlg->update();
        break;
        case POST_CARD_CONTINUE:
        chupai->move(gamingdlg->width()*0.5-chupai->width()*0.5+40,gamingdlg->height()*0.8-150);
        chupai->show();
        qipai->move(gamingdlg->width()*0.5-qipai->width()*0.5+200,gamingdlg->height()*0.8-150);
        qipai->show();
        gamingdlg->update();
        break;
    }
}

//处理回合开始
void CKernel::SLOT_DealTurnBeginRs(char *buf,int nlen){
    b_isKill = false;
    if(turnlogo){
        turnlogo->hide();
        gamingdlg->update();
    }
    //发送请求抽卡包
    STRU_TURN_BEGIN *rs = (STRU_TURN_BEGIN *)buf;
    if(rs->user_id == this->m_id){
        chupai->move(gamingdlg->width()*0.5-chupai->width()*0.5+40,gamingdlg->height()*0.8-150);
        chupai->setParent(gamingdlg);
        chupai->show();
        qipai->move(gamingdlg->width()*0.5-qipai->width()*0.5+200,gamingdlg->height()*0.8-150);
        qipai->setParent(gamingdlg);
        qipai->show();
        gamingdlg->update();
        STRU_GETCARD_RQ rq;
        rq.m_roomid = this->m_roomid;
        rq.m_userid = this->m_id;
        rq.num = 2;
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
    }else{
        chupai->hide();
        qipai->hide();
        gamingdlg->update();
    }

    //设置头像框
    int curseatid = FindSeatIdById(rs->user_id);
    turnlogo->setParent(gamingdlg);
    if(curseatid == this->MySeatId){
        turnlogo->move(gamingdlg->width()*0.5-turnlogo->width()*0.5-445,gamingdlg->height()*0.8-55);
    }else{
        turnlogo->move(this->m_mapSeatIdToPosition[curseatid][0]+55,this->m_mapSeatIdToPosition[curseatid][1]);
    }
    turnlogo->show();
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
    this->myhp = this->m_mapSeatIdToHp[this->MySeatId];
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
        gamingdlg->update();
        ++ite;
    }
    gamingdlg->update();
}

//显示所有人选择的英雄
void CKernel::ShowHero(){
    auto ite = this->m_mapSeatIdToHeroId.begin();
    while(ite!= this->m_mapSeatIdToHeroId.end()){
        //如果
        if((*ite).first != this->MySeatId && (*ite).second != this->ZG_HeroId){
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
                    this->b_choosefirstpeople = false;
                }

            });
            gamingdlg->update();
        }
        ++ite;
    }
}

int CKernel::FindSeatIdById(int myid){
    //QMessageBox::about(gamingdlg,"ttt","sss");
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
        connect(hero,&MyPushButton::clicked,[=](){
            if(!this->b_choosefirstpeople){
                //如果没选择第一个人
                this->usecardtoid1 = this->m_mapSeatIdToId[hero->seatid];
                this->b_choosefirstpeople = true;
            }else{
                //如果选择了第一个人
                this->usecardtoid2 = this->m_mapSeatIdToId[hero->seatid];
                this->b_choosefirstpeople = false;
            }
        });
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
    QTimer::singleShot(3000,this,[=]{
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
        m_mapSeatIdToLength[seatid] = 1;
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
        m_mapSeatIdToLength[seatid] = 2;
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
        m_mapSeatIdToLength[seatid] = 2;
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
        m_mapSeatIdToLength[seatid] = 1;
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
