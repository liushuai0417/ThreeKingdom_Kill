#ifndef HEROBUTTON_H
#define HEROBUTTON_H

#include <QWidget>
#include<QPushButton>
#include<QMouseEvent>
class HeroButton : public QPushButton
{
    Q_OBJECT
public:
    //explicit HeroButton(QWidget *parent = 0);
    //重写构造函数 normalImg按钮按下前的图片路径 pressImg按钮按下后的图片路径
    HeroButton(QString normalImg,QString pressImg="");
    //重写按钮 按下和释放事件
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void zoom1();//向下跳
    void zoom2();//向上跳
    void ChooseHero();//选择牌
    void ChooseHero1();//选择牌
public:
    bool b_flagchoose;//是否是选中状态
    int chooseheroid;
signals:

public slots:
public:
    QString heroname;//英雄名字
    int index;
    int seatid;
private:
    QString normalImgPath;//按钮按下前的图片路径
    QString pressImgPath;//按钮按下后的图片路径
};

#endif // HEROBUTTON_H
