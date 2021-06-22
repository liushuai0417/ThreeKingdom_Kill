#ifndef ROOMITEM_H
#define ROOMITEM_H

#include <QWidget>
#include<QMouseEvent>
#include<QPaintEvent>
namespace Ui {
class RoomItem;
}

class RoomItem : public QWidget
{
    Q_OBJECT
signals:
    void SIG_JoinRoom(QString roomid);
public:
    explicit RoomItem(QWidget *parent = 0);
    ~RoomItem();
    void mouseDoubleClickEvent(QMouseEvent *event);
    void setItem(int Roomid,QString RoomName,QString CreatorName,int num);
    void paintEvent(QPaintEvent *event);
    Ui::RoomItem *getUi() const;
private:
    Ui::RoomItem *ui;
    int m_RoomID;
    QString m_RoomName;
    QString m_CreatorName;
};

#endif // ROOMITEM_H
