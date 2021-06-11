#ifndef JOINROOMDIALOG_H
#define JOINROOMDIALOG_H

#include <QDialog>
#include<QPaintEvent>
#include"packdef.h"
#include<QVBoxLayout>
#include<QCloseEvent>
#include<vector>
#include"roomitem.h"
namespace Ui {
class JoinRoomDialog;
}

class JoinRoomDialog : public QDialog
{
    Q_OBJECT
signals:
    void SIG_JoinRoomByIdCommit(QString content);
    void SIG_JoinRoomByNameCommit(QString content);
    void SIG_SetCountZero();//向kernel发送将count置0信号
public:
    explicit JoinRoomDialog(QWidget *parent = 0);
    ~JoinRoomDialog();
    void paintEvent(QPaintEvent *event);
    void Slot_AddRoomItem(QWidget *item);
    void Slot_RemoveRoomItem(QWidget *item);
    void closeEvent(QCloseEvent *event);
    vector<RoomItem*>vec;
    QVBoxLayout *m_roomLayout;
public slots:
    void SLOT_JoinRoomById();
    void SLOT_JoinRoomByName();

private:
    Ui::JoinRoomDialog *ui;
};

#endif // JOINROOMDIALOG_H
