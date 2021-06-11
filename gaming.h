#ifndef GAMING_H
#define GAMING_H

#include <QWidget>
#include<QPaintEvent>
namespace Ui {
class Gaming;
}

class Gaming : public QWidget
{
    Q_OBJECT

public:
    explicit Gaming(QWidget *parent = 0);
    ~Gaming();
    void paintEvent(QPaintEvent *event);
private:
    Ui::Gaming *ui;
};

#endif // GAMING_H
