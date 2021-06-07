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
    void setItem(int iconid,QString name,QString feeling);
    void paintEvent(QPaintEvent *event);
private:
    Ui::FriendItem *ui;
};

#endif // FRIENDITEM_H
