#ifndef ROOMITEM_H
#define ROOMITEM_H

#include <QWidget>
#include<QMouseEvent>
namespace Ui {
class RoomItem;
}

class RoomItem : public QWidget
{
    Q_OBJECT

public:
    explicit RoomItem(QWidget *parent = 0);
    ~RoomItem();
    void mouseDoubleClickEvent(QMouseEvent *event);
    void setItem(int Roomid,QString RoomName,QString CreatorName);
private:
    Ui::RoomItem *ui;
    int m_RoomID;
    QString m_RoomName;
    QString m_CreatorName;
};

#endif // ROOMITEM_H