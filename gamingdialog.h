#ifndef GAMINGDIALOG_H
#define GAMINGDIALOG_H

#include <QDialog>
#include<QPaintEvent>
namespace Ui {
class GamingDialog;
}

class GamingDialog : public QDialog
{
    Q_OBJECT
signals:
    void SIG_LeaveRoom(int roomid);
public:
    explicit GamingDialog(QWidget *parent = 0);
    ~GamingDialog();
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);
    int roomid;
private:
    Ui::GamingDialog *ui;
};

#endif // GAMINGDIALOG_H
