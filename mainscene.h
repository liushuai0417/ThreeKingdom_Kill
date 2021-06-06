#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QMainWindow>
#include<QPaintEvent>
#include<QVBoxLayout>
namespace Ui {
class MainScene;
}

class MainScene : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainScene(QWidget *parent = 0);
    ~MainScene();
    void paintEvent(QPaintEvent *event);
    Ui::MainScene *getUi() const;
signals:
    void SIG_ShowAlterInfo();//通知kernel类显示修改信息的窗口
    void SIG_CreateRoom();//通知kernel类显示创建房间的窗口
    void SIG_AddFriend();//通知kernel类显示添加好友的窗口
    void SIG_JoinRoom();//通知kernel类显示加入房间的窗口
    void SIG_ReGetRoomTable();//通知kernel类获取房间列表
private:
    Ui::MainScene *ui;
    QVBoxLayout *m_mainLayout;
public slots:
    void Slot_CreateRoom();//创建房间槽函数
    void Slot_AddFriend();//添加好友槽函数
    void Slot_JoinRoom();//加入房间槽函数
    void Slot_AlterInfo();//修改信息槽函数
    void Slot_StartGame();//开始游戏槽函数
    void Slot_AddUserItem(QWidget *item);
    void Slot_RemoveUserItem(QWidget *item);
    void Slot_RegetRoom();//获取房间列表槽函数
};

#endif // MAINSCENE_H
