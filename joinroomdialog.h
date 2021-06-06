#ifndef JOINROOMDIALOG_H
#define JOINROOMDIALOG_H

#include <QDialog>
#include<QPaintEvent>
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
public slots:
    void SLOT_JoinRoomById();
    void SLOT_JoinRoomByName();
private:
    Ui::JoinRoomDialog *ui;
};

#endif // JOINROOMDIALOG_H
