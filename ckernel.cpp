#include "ckernel.h"
#include"packdef.h"
#include<QDebug>
#include<QMessageBox>
#include<QLatin1String>
#include<QThread>
#include "ui_dialog.h"
#include"roomitem.h"
#include "ui_mainscene.h"
#include"addfrienddialog.h"
#include"frienditem.h"
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
    friendlistDialog = new FriendList;
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

    //获取房间列表槽函数
    connect(m_Dialog,&Dialog::SIG_AskRoomCommit,[=](){
        STRU_ASKROOM_RQ rq;
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
    });

    //修改信息的槽函数
    connect(changeDialog,AlterInfoSignal,[=](int iconid,QString name,QString feeling){
        //发送修改信息的包

    });

    //创建房间的槽函数
    connect(createDialog,CreateRoomSignal,[=](QString roomname){
        qDebug()<<"发送创建房间";
        STRU_CREATEROOM_RQ rq;
        strcpy(rq.m_RoomName,roomname.toStdString().c_str());
        strcpy(rq.m_szUser,this->m_szName.toStdString().c_str());
        rq.m_userid = this->m_id;
        m_tcpClient->SendData((char*)&rq,sizeof(rq));
    });

    //通过name查找好友
    connect(addDialog,AddFriendByNameSignal,[=](QString content){
        //发送查找好友包
        qDebug()<<"name查找好友"<<content;
    });

    //通过name查找房间
    connect(joinDialog,JoinRoomByNameSignal,[=](QString content){
        //发送查找房间包
        qDebug()<<"name查找房间"<<content;
    });

    //通过id查找房间
    connect(joinDialog,JoinRoomByIdSignal,[=](QString content){
        //发送查找房间包
        qDebug()<<"id查找房间"<<content;
    });

    connect(m_MainScene,&MainScene::SIG_ReGetRoomTable,this,&CKernel::SLOT_ReGetRoomTable);
    connect(m_MainScene,&MainScene::SIG_ShowAlterInfo,this,&CKernel::SLOT_ShowAlterInfo);
    connect(m_MainScene,&MainScene::SIG_CreateRoom,this,&CKernel::SLOT_ShowCreateRoom);
    connect(m_MainScene,&MainScene::SIG_AddFriend,this,&CKernel::SLOT_ShowAddFriend);
    connect(m_MainScene,&MainScene::SIG_JoinRoom,this,&CKernel::SLOT_ShowJoinRoom);
    connect(m_MainScene,&MainScene::SIG_GetFriendList,this,&CKernel::SLOT_GetFriendList);
    //房间列表的第一行
    RoomItem *itemindex = new RoomItem;
    m_MainScene->Slot_AddUserItem(itemindex);


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
    qDebug()<<__func__<<DEF_PACK_LOGIN_RS-DEF_PACK_BASE;
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
            this->m_id = rs->m_userid;
            this->m_iconID = rs->m_userInfo.m_iconID;
            this->m_szName = QString(rs->m_userInfo.m_szName);
            this->m_feeling = QString(rs->m_userInfo.m_feeling);
            this->m_state = rs->m_userInfo.m_state;
            m_MainScene->getUi()->lb_name->setText(m_szName);
            m_MainScene->getUi()->lb_feeling->setText(m_feeling);
            QPixmap pix;
            QString strPath;
            if(m_iconID<10){
                strPath = QString(":/res/TX/0%1.png").arg(m_iconID);
            }else{
                strPath = QString(":/res/TX/%1.png").arg(m_iconID);
            }
            pix.load(strPath);
            m_MainScene->getUi()->pb_icon->setFixedSize(pix.width(),pix.height());
            m_MainScene->getUi()->pb_icon->setStyleSheet("QPushButton{border:0px;}");//边框设置为0像素
            m_MainScene->getUi()->pb_icon->setIcon(pix);
            m_MainScene->getUi()->pb_icon->setIconSize(QSize(pix.width(),pix.height()));
            //登录成功
            QMessageBox::about(m_Dialog,"提示","登录成功");
            m_Dialog->hide();
            //m_MainScene = new MainScene;//大厅指针
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
                RoomItem *item = new RoomItem;
                vec_roomitem.push_back(item);
                qDebug()<<"数组信息"<<roomname<<roomid<<roomcreator;
                item->setItem(roomid,roomname,roomcreator);
                m_MainScene->Slot_AddUserItem(item);
                i++;
            }

        }
        break;
        case ask_room_failed:
            QMessageBox::about(m_MainScene,"提示","获取房间列表失败");
        break;
    }
}

void CKernel::SLOT_DealCreateRoom(char *buf,int nlen){
    STRU_CREATEROOM_RS *rs = (STRU_CREATEROOM_RS *)buf;
    QString str;
    switch (rs->m_lResult) {
        case create_success:{
            str = QString("创建房间成功,房间号为%1").arg(rs->m_RoomId);
            QMessageBox::about(m_MainScene,"提示",str);
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

void CKernel::SLOT_ShowJoinRoom(){
    joinDialog->show();
}

void CKernel::SLOT_ReGetRoomTable(){
    auto ite = vec_roomitem.begin();
    while(ite!=vec_roomitem.end()){
        m_MainScene->Slot_RemoveUserItem(*ite);
        delete *ite;
        *ite = NULL;
        ite++;
    }
    m_MainScene->repaint();
    STRU_ASKROOM_RQ rq;
    m_tcpClient->SendData((char*)&rq,sizeof(rq));
}

void CKernel::SLOT_GetFriendList(){

    FriendItem *item1 = new FriendItem;
    item1->setItem(1,"test1","测试1");
    FriendItem *item2 = new FriendItem;
    item2->setItem(2,"test1","测试1");
    FriendItem *item3 = new FriendItem;
    item3->setItem(3,"test1","测试1");
    FriendItem *item4 = new FriendItem;
    item4->setItem(4,"test1","测试1");
    friendlistDialog->Slot_AddFriendItem(item1);
    friendlistDialog->Slot_AddFriendItem(item2);
    friendlistDialog->Slot_AddFriendItem(item3);
    friendlistDialog->Slot_AddFriendItem(item4);
    friendlistDialog->show();
    //发送查询好友列表包

}
