#ifndef CREATEROOMDIALOG_H
#define CREATEROOMDIALOG_H

#include <QDialog>
#include<QPaintEvent>
namespace Ui {
class CreateRoomDialog;
}

class CreateRoomDialog : public QDialog
{
    Q_OBJECT
signals:
    void SIG_CreateRoomCommit(QString roomname);
public:
    explicit CreateRoomDialog(QWidget *parent = 0);
    ~CreateRoomDialog();
    void paintEvent(QPaintEvent *event);
public slots:
    void SLOT_CreatRoom();
private:
    Ui::CreateRoomDialog *ui;
};

#endif // CREATEROOMDIALOG_H
