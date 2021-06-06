#ifndef CHANEGINFODIALOG_H
#define CHANEGINFODIALOG_H

#include <QDialog>
#include<QPaintEvent>
namespace Ui {
class ChanegInfoDialog;
}

class ChanegInfoDialog : public QDialog
{
    Q_OBJECT
signals:
    void SIG_AleterInfoCommit(int iconid,QString name,QString feeling);
public slots:
    void SLOT_AlterInfo();//修改信息槽函数
public:
    explicit ChanegInfoDialog(QWidget *parent = 0);
    ~ChanegInfoDialog();
    void paintEvent(QPaintEvent *event);
private:
    Ui::ChanegInfoDialog *ui;
};

#endif // CHANEGINFODIALOG_H
