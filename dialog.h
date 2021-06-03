#ifndef DIALOG_H
#define DIALOG_H
#include<QPaintEvent>
#include <QDialog>
#include<mainscene.h>
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    void paintEvent(QPaintEvent *event);
private:
    Ui::Dialog *ui;

signals:
    void SIG_CLOSE();//关闭的信号
    void SIG_RegisterCommit(QString username,QString email,QString password);
    void SIG_LoginCommit(QString username,QString password);
};

#endif // DIALOG_H
