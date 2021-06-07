#ifndef JOINROOMDIALOG_H
#define JOINROOMDIALOG_H

#include <QDialog>
#include<QPaintEvent>
#include"packdef.h"
#include<QVBoxLayout>
namespace Ui {
class JoinRoomDialog;
}

class JoinRoomDialog : public QDialog
{
    Q_OBJECT
signals:
    void SIG_JoinRoomByIdCommit(QString content);
    void SIG_JoinRoomByNameCommit(QString content);
public:
    explicit JoinRoomDialog(QWidget *parent = 0);
    ~JoinRoomDialog();
    void paintEvent(QPaintEvent *event);
    void Slot_AddFriendItem(QWidget *item);
    void Slot_RemoveFriendItem(QWidget *item);
    QVBoxLayout *m_roomLayout;
public slots:
    void SLOT_JoinRoomById();
    void SLOT_JoinRoomByName();

private:
    Ui::JoinRoomDialog *ui;
};

#endif // JOINROOMDIALOG_H
