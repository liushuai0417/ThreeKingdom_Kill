#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QMainWindow>
#include<QPaintEvent>
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
private:
    Ui::MainScene *ui;

public slots:
    void Slot_CreateRoom();//创建房间槽函数
    void Slot_AddFriend();//添加好友槽函数
    void Slot_JoinRoom();//加入房间槽函数
    void Slot_AlterInfo();//修改信息槽函数
    void Slot_StartGame();//开始游戏槽函数
};

#endif // MAINSCENE_H
