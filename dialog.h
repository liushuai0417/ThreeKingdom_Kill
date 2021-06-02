#ifndef DIALOG_H
#define DIALOG_H
#include<QPaintEvent>
#include <QDialog>

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
};

#endif // DIALOG_H
