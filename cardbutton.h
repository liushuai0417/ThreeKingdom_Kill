#ifndef CARDBUTTON_H
#define CARDBUTTON_H

#include <QWidget>
#include<QPushButton>
#include<QMouseEvent>
class CardButton : public QPushButton
{
    Q_OBJECT
public:
    //explicit CardButton(QWidget *parent = 0);
    CardButton(QString normalImg,QString pressImg="");
    //重写按钮 按下和释放事件
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void zoom1();//向下跳
    void zoom2();//向上跳
    void ChooseHero();//选择牌
    void ChooseHero1();//选择牌
public:
    bool b_flagchoose;//是否是选中状态
    int color;//花色
    int num;//点数
    int type;//牌类型
    int id;//牌id
signals:
private:
    QString normalImgPath;//按钮按下前的图片路径
    QString pressImgPath;//按钮按下后的图片路径
public slots:
};

#endif // CARDBUTTON_H
