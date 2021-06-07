#ifndef ADDFRIENDDIALOG_H
#define ADDFRIENDDIALOG_H

#include <QDialog>
#include<QPaintEvent>
#include<QVBoxLayout>
namespace Ui {
class AddFriendDialog;
}

class AddFriendDialog : public QDialog
{
    Q_OBJECT
signals:
    void SIG_AddFriendByNameCommit(QString content);
public slots:
    void SLOT_AddFriendByName();
public:
    explicit AddFriendDialog(QWidget *parent = 0);
    ~AddFriendDialog();
    void paintEvent(QPaintEvent *event);
    void Slot_AddFriendItem(QWidget *item);
    void Slot_RemoveFriendItem(QWidget *item);
    QVBoxLayout *m_friendLayout;

private:
    Ui::AddFriendDialog *ui;

};

#endif // ADDFRIENDDIALOG_H
