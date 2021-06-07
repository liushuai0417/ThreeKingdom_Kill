#ifndef FRIENDITEM_H
#define FRIENDITEM_H

#include <QWidget>
#include<QPaintEvent>
namespace Ui {
class FriendItem;
}

class FriendItem : public QWidget
{
    Q_OBJECT

public:
    explicit FriendItem(QWidget *parent = 0);
    ~FriendItem();
    void setItem(int iconid,QString name,QString feeling,int state);
    void paintEvent(QPaintEvent *event);
    int m_friendId;//用户id
    QString m_friendName;//用户昵称
private:
    Ui::FriendItem *ui;
};

#endif // FRIENDITEM_H
