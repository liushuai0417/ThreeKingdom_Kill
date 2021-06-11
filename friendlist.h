#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include <QMainWindow>
#include<QVBoxLayout>
namespace Ui {
class FriendList;
}

class FriendList : public QMainWindow
{
    Q_OBJECT
signals:
    void SIG_ReGetFriendList();
public:
    explicit FriendList(QWidget *parent = 0);
    ~FriendList();
    QVBoxLayout *m_friendLayout;
public slots:
    void Slot_AddFriendItem(QWidget *item);
    void Slot_RemoveFriendItem(QWidget *item);
    void paintEvent(QPaintEvent *event);
private:
    Ui::FriendList *ui;
};

#endif // FRIENDLIST_H
