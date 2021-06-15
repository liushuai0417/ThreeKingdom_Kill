#ifndef __DEF_PACKDEF__
#define __DEF_PACKDEF__
#include<QHostAddress>
#include<QByteArray>
#include<QDebug>
#include<QFile>
#include<QDateTime>
#include <winsock2.h>

using namespace std;


#define BOOL bool
#define DEF_PACK_BASE  (10000)


typedef enum Net_PACK
{
    DEF_PACK_REGISTER_RQ = 10000,               //注册请求
    DEF_PACK_REGISTER_RS,

    DEF_PACK_LOGIN_RQ,                          //登录请求
    DEF_PACK_LOGIN_RS,

    DEF_PACK_GETFRIENDLIST_RQ,                  //获取好友列表请求
    DEF_PACK_GETFRIENDLIST_RS,

    DEF_PACK_CREATEROOM_RQ,                     //创建房间请求
    DEF_PACK_CREATEROOM_RS,

    DEF_PACK_JOINROOM_RQ ,                      //加入房间请求
    DEF_PACK_JOINROOM_RS,

    DEF_PACK_SEARCH_ROOM_RQ,                    //查找房间请求
    DEF_PACK_SEARCH_ROOM_RS,

    DEF_PACK_ROOM_MEMBER_RQ,                    //请求房间成员
    DEF_PACK_ROOM_MEMBER_RS,

    DEF_PACK_ASKROOM_RQ,                        //刷新房间列表请求
    DEF_PACK_ASKROOM_RS,

    DEF_PACK_CHECKOFFLINE_RQ,                   //查询离线数据请求
    DEF_PACK_CHECKOFFLINE_RS,

    DEF_PACK_STARTGAME_RQ,                      //开始游戏请求
    DEF_PACK_STARTGAME_RS,


    DEF_PACK_LEAVEROOM_RQ ,                     //离开房间请求
    DEF_PACK_LEAVEROOM_RS ,

    DEF_PACK_SEARCH_FRIEND_RQ,                  //查询好友请求
    DEF_PACK_SEARCH_FRIEND_RS,

    DEF_PACK_ADD_FRIEND_RQ,                     //添加好友请求
    DEF_PACK_ADD_FRIEND_RS,

    DEF_ALTER_USERINFO_RQ,                      //修改用户信息请求
    DEF_ALTER_USERINFO_RS,

    DEF_PACK_CHAT_RQ,                           //聊天请求
    DEF_PACK_CHAT_RS,

    DEF_PACK_OFFLINRE_RQ,                       //下线请求
    DEF_PACK_OFFLINRE_RS,
}Net_PACK;

//注册请求结果
#define userid_is_exist      0
#define register_sucess      1

//登录请求结果
#define userid_no_exist      0
#define password_error       1
#define login_sucess         2
#define user_online          3

//查看好友列表结果
#define aks_failed           0
#define ask_success          1

//创建房间结果
#define create_failed        0
#define create_success       1

//查找房间结果
#define search_room_failed   0
#define search_room_success  1

//开始游戏结果
#define game_start_success   1

//加入房间结果
#define room_no_exist        0
#define room_is_full         1
#define join_success         2

//请求房间列表结果
#define ask_room_failed     0
#define ask_room_success    1

//查询好友结果
#define no_this_user        0
#define search_success      1
#define is_your_friend      2

//添加好友结果
#define add_failed          0
#define add_success         1

//修改用户信息结果
#define name_repeat      0      //修改失败名称重复
#define alter_success    1

#define DEF_PACK_COUNT (100)

#define MAX_PATH            (280 )
#define MAX_SIZE            (60  )
#define MAX_ROOMLIST        (25)
#define DEF_HOBBY_COUNT     (8  )
#define MAX_CONTENT_LEN     (4096 )
#define _DEF_PORT (8000)
#define MAX_ROOM_COUNT      (128)
#define MAX_ROOM_INFO_COUNT (64)
/////////////////////网络//////////////////////////////////////


#define DEF_MAX_BUF	  1024
#define DEF_BUFF_SIZE	  4096


typedef int PackType;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //用户信息
        typedef struct STRU_USER_INFO
        {
            STRU_USER_INFO()
            {
                m_userid = 0;
                m_iconid = 0;
                memset(m_szName,0,MAX_SIZE);
                memset(m_szFelling,0,MAX_SIZE);
                status = 0;
            }
            int m_userid;
            int m_iconid;
            char m_szName[MAX_SIZE];
            char m_szFelling[MAX_SIZE];
            int status;
        }STRU_USER_INFO;


//注册请求
typedef struct STRU_REGISTER_RQ
{
    STRU_REGISTER_RQ()
    {
        m_nType = DEF_PACK_REGISTER_RQ;
        memset(m_szUser,0,MAX_SIZE);
        memset(m_szPassword,0,MAX_SIZE);
        memset(m_szEmall,0,MAX_SIZE);
    }

    PackType m_nType;   //包类型
    char     m_szUser[MAX_SIZE] ; //用户名
    char     m_szEmall[MAX_SIZE]; //邮箱
    char     m_szPassword[MAX_SIZE];  //密码

}STRU_REGISTER_RQ;


//注册回复
typedef struct STRU_REGISTER_RS
{
    STRU_REGISTER_RS()
    {
        m_nType= DEF_PACK_REGISTER_RS;
        m_lResult = 0;
    }
    PackType m_nType;   //包类型
    int  m_lResult ; //注册结果

}STRU_REGISTER_RS;


//登录请求
typedef struct STRU_LOGIN_RQ
{
    STRU_LOGIN_RQ()
    {
        m_nType = DEF_PACK_LOGIN_RQ;
        memset(m_szUser,0,MAX_SIZE);
        memset(m_szPassword,0,MAX_SIZE);
    }

    PackType m_nType;   //包类型
    char     m_szUser[MAX_SIZE] ; //用户名
    char     m_szPassword[MAX_SIZE];  //密码
}STRU_LOGIN_RQ;


//登录回复
typedef struct STRU_LOGIN_RS
{
    STRU_LOGIN_RS()
    {
        m_nType= DEF_PACK_LOGIN_RS;
        m_lResult = 0;
    }
    PackType m_nType;   //包类型
    int  m_lResult ; //注册结果
    STRU_USER_INFO m_userInfo;

}STRU_LOGIN_RS;


//获取好友列表请求
typedef struct STRU_GETFRILIST_RQ
{
    STRU_GETFRILIST_RQ()
    {
        m_nType = DEF_PACK_GETFRIENDLIST_RQ;
        m_userid = 0;
    }
    PackType m_nType;
    int m_userid;
}STRU_GETFRILIST_RQ;


//获取好友列表回复
typedef struct STRU_GETFRILIST_RS
{
    STRU_GETFRILIST_RS()
    {
        m_nType = DEF_PACK_GETFRIENDLIST_RS;
        m_lResult = 0;
    }
    PackType m_nType;
    int m_lResult;
    STRU_USER_INFO m_friArr[MAX_SIZE];    //最后一个好友id为-1

}STRU_GETFRILIST_RS;


//查找好友请求
typedef struct STRU_SEARCH_FRIEND_RQ
{
    STRU_SEARCH_FRIEND_RQ()
    {
        m_nType = DEF_PACK_SEARCH_FRIEND_RQ;
        m_userid = 0;
        memset(sz_friendName,0,sizeof(sz_friendName));
    }
    PackType m_nType;
    int m_userid; //自身id
    char sz_friendName[MAX_SIZE];
}STRU_SEARCH_FRIEND_RQ;


//查找好友回复
typedef struct STRU_SEARCH_FRIEND_RS
{
    STRU_SEARCH_FRIEND_RS()
    {
        m_nType = DEF_PACK_SEARCH_FRIEND_RS;
        m_result = 0;
    }
    PackType m_nType;
    int m_result;
    STRU_USER_INFO m_friInfo;
}STRU_SEARCH_FRIEND_RS;


//添加好友请求
typedef struct STRU_ADD_FRIEND_RQ
{
    STRU_ADD_FRIEND_RQ()
    {
        m_nType = DEF_PACK_ADD_FRIEND_RQ;
    }
    PackType   m_nType;
    STRU_USER_INFO  m_userInfo;     //自身用户信息
    STRU_USER_INFO  m_friInfo;      //好友用户信息
}STRU_ADD_FRIEND_RQ;


//添加好友回复
typedef struct STRU_ADD_FRIEND_RS
{
    STRU_ADD_FRIEND_RS()
    {
        m_nType = DEF_PACK_ADD_FRIEND_RS;
        m_result = 0;
    }
    PackType   m_nType;   //包类型
    int m_result;
    STRU_USER_INFO  m_userInfo;     // 好友信息
    STRU_USER_INFO  m_friInfo;      // 自身信息(被添加好友)
}STRU_ADD_FRIEND_RS;


//修改个人信息请求
typedef struct STRU_ALTER_USERINFO_RQ
{
    STRU_ALTER_USERINFO_RQ()
    {
        m_nType = DEF_ALTER_USERINFO_RQ;
    }
    PackType m_nType;
    STRU_USER_INFO  m_userInfo;

}STRU_ALTER_USERINFO_RQ;

//修改个人信息回复
typedef struct STRU_ALTER_USERINFO_RS
{
    STRU_ALTER_USERINFO_RS()
    {
        m_nType = DEF_ALTER_USERINFO_RS;
        m_result = 0;
    }
    PackType m_nType;
    int m_result;
}STRU_ALTER_USERINFO_RS;

////聊天请求
//typedef struct STRU_CHAT_RQ
//{
//    STRU_CHAT_RQ()
//    {
//        m_nType = DEF_PACK_CHAT_RQ;
//        m_userID = 0;
//        m_friendID = 0;
//        memset(m_ChatContent,0,MAX_CONTENT_LEN);
//    }
//    PackType   m_nType;   //包类型
//    char m_userID;
//    char m_friendID;
//    char m_ChatContent[MAX_CONTENT_LEN];

//}STRU_CHAT_RQ;

////聊天回复
//typedef struct STRU_CHAT_RS
//{
//    STRU_CHAT_RS()
//    {
//        m_nType = DEF_PACK_CHAT_RS;
//        m_friendID = 0;
//        m_userID = 0;
//    }
//    PackType   m_nType;   //包类型
//    int m_userID;
//    int m_friendID;
//    int m_result;

//}STRU_CHAT_RS;


//房间信息
typedef struct RoomInfo
{
    RoomInfo()
    {
        m_Roomid = 0;
        memset(sz_Roomname,0,MAX_SIZE);
        memset(sz_RoomCreator,0,MAX_SIZE);
        m_num = 0;
    }
    int m_Roomid;
    char sz_Roomname[MAX_SIZE];
    char sz_RoomCreator[MAX_SIZE];
    int m_num;
}RoomInfo;


//创建房间请求
typedef struct STRU_CREATEROOM_RQ
{
    STRU_CREATEROOM_RQ()
    {
         m_nType = DEF_PACK_CREATEROOM_RQ;
         memset(m_szUser,0,MAX_SIZE);
         memset(m_RoomName,0,MAX_SIZE);
         m_userid = 0;
    }
    PackType m_nType;   //包类型
    int     m_userid ;                    //房主id
    char m_szUser[MAX_SIZE];
    char m_RoomName[MAX_SIZE];            //房间名
}STRU_CREATEROOM_RQ;


//创建房间回复
typedef struct STRU_CREATEROOM_RS
{
    STRU_CREATEROOM_RS()
    {
        m_nType= DEF_PACK_CREATEROOM_RS;
        m_lResult = 0;
        m_RoomId = 0;
    }
    PackType m_nType;   //包类型
    int  m_lResult ;    //注册结果
    int  m_RoomId;      //房间id
}STRU_CREATEROOM_RS;


//刷新房间列表请求
typedef struct STRU_ASKROOM_RQ
{
    STRU_ASKROOM_RQ()
    {
         m_nType = DEF_PACK_ASKROOM_RQ;
    }
    PackType m_nType;

}STRU_ASKROOM_RQ;


//刷新房间回复
typedef struct STRU_ASKROOM_RS
{
    STRU_ASKROOM_RS()
    {
        m_nType = DEF_PACK_ASKROOM_RS;
        m_lResult = 0;
        memset(m_RoomList,0,sizeof(RoomInfo)*MAX_ROOMLIST);
    }
    PackType m_nType;
    int  m_lResult ;
    RoomInfo m_RoomList[MAX_ROOMLIST];
}STRU_ASKROOM_RS;


//查找房间请求
typedef struct STRU_SEARCH_ROOM_RQ
{
    STRU_SEARCH_ROOM_RQ()
    {
        m_nType = DEF_PACK_SEARCH_ROOM_RQ;
        m_Roomid = 0;
        memset(m_szRoomName,0,MAX_SIZE);
    }
    PackType m_nType;
    int m_Roomid;
    char m_szRoomName[MAX_SIZE];

}STRU_SEARCH_ROOM_RQ;


//查找房间回复
typedef struct STRU_SEARCH_ROOM_RS
{
    STRU_SEARCH_ROOM_RS()
    {
        m_nType = DEF_PACK_SEARCH_ROOM_RS;
    }
    PackType m_nType;
    int m_lResult;
    RoomInfo m_roomInfo;
}STRU_SEARCH_ROOM_RS;


//加入房间请求
typedef struct STRU_JOINROOM_RQ
{
    STRU_JOINROOM_RQ()
    {
        m_nType = DEF_PACK_JOINROOM_RQ;
        m_RoomID = 0;
    }
    PackType m_nType;   //包类型
    STRU_USER_INFO m_userInfo;
    int m_RoomID;

}STRU_JOINROOM_RQ;


//加入房间回复
typedef struct STRU_JOINROOM_RS
{
    STRU_JOINROOM_RS()
    {
        m_nType= DEF_PACK_JOINROOM_RS;
        m_lResult = 0;
    }
    PackType m_nType;       //包类型
    int  m_lResult ;
    STRU_USER_INFO m_userInfoarr[4];   //房间用户id
}STRU_JOINROOM_RS;


//房间成员请求
typedef struct STRU_ROOM_MEMBER_RQ
{
    STRU_ROOM_MEMBER_RQ()
    {
        m_nType= DEF_PACK_ROOM_MEMBER_RQ;
        m_userid = 0;
        m_RoomID = 0;
    }
    PackType m_nType;   //包类型
    int m_userid;       //自己用户id
    int m_RoomID;

}STRU_ROOM_MEMBER_RQ;

//房间成员回复
typedef struct STRU_ROOM_MEMBER_RS
{
    STRU_ROOM_MEMBER_RS()
    {
         m_nType= DEF_PACK_ROOM_MEMBER_RS;
    }
    PackType m_nType;   //包类型
    STRU_USER_INFO m_userInfo[4];
}STRU_ROOM_MEMBER_RS;

//离开房间请求
typedef struct STRU_LEAVEROOM_RQ
{
    STRU_LEAVEROOM_RQ()
    {
        m_nType = DEF_PACK_LEAVEROOM_RQ;
        m_userId = 0;
        m_RoomId = 0;
    }
    PackType   m_nType;   //包类型
    int    m_userId;   //用户ID
    int     m_RoomId;
}STRU_LEAVEROOM_RQ;


//离开房间回复
typedef struct STRU_LEAVEROOM_RS
{
    STRU_LEAVEROOM_RS()
    {
    m_nType = DEF_PACK_LEAVEROOM_RS;
    m_lResult = 0 ;
    }
    PackType   m_nType;   //包类型
    int  m_lResult ;
}STRU_LEAVEROOM_RS;

//游戏开始请求
typedef struct STRU_STARTGAME_RQ
{
    STRU_STARTGAME_RQ()
    {
        m_nType = DEF_PACK_STARTGAME_RQ;
        Room_id = 0;
        user_id = 0;
    }
    PackType m_nType;
    int Room_id;
    int user_id;
}STRU_STARTGAME_RQ;

//游戏开始回复
typedef struct STRU_STARTGAME_RS
{
    STRU_STARTGAME_RS()
    {
       m_nType = DEF_PACK_STARTGAME_RS;
       m_lResult = 0;
    }

    PackType m_nType;
    int m_lResult;
}STRU_STARTGAME_RS;

//查询离线数据请求
typedef struct STRU_CHECK_OFFLINEMSG_RQ
{
    STRU_CHECK_OFFLINEMSG_RQ()
    {
        m_nType = DEF_PACK_CHECKOFFLINE_RQ;
    }
    PackType m_nType;
    STRU_USER_INFO m_userInfo;
}STRU_CHECK_OFFLINEMSG_RQ;


//离线请求
typedef struct STRU_OFFLINE_RQ
{
    STRU_OFFLINE_RQ()
    {
        m_nType = DEF_PACK_OFFLINRE_RQ;
        m_userID = 0;
    }
    PackType   m_nType;   //包类型
    int m_userID;

}STRU_OFFLINE_RQ;


/*typedef struct STRU_FORCE_OFFLINE
{
    STRU_FORCE_OFFLINE()
    {
        m_nType = DEF_PACK_FORCE_OFFLINE;
        m_UserID = 0;
    }
    PackType   m_nType;   //包类型
    int m_UserID;

}STRU_FORCE_OFFLINE*/;


#endif
