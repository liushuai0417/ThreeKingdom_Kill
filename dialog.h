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
    MainScene *mainScene = NULL;//游戏大厅的指针
};

#endif // DIALOG_H
