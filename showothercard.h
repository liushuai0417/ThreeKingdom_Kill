#ifndef SHOWOTHERCARD_H
#define SHOWOTHERCARD_H

#include <QDialog>
#include<QPaintEvent>
namespace Ui {
class ShowOtherCard;
}

class ShowOtherCard : public QDialog
{
    Q_OBJECT

public:
    explicit ShowOtherCard(QWidget *parent = 0);
    ~ShowOtherCard();
    void paintEvent(QPaintEvent *event);
private:
    Ui::ShowOtherCard *ui;
};

#endif // SHOWOTHERCARD_H
