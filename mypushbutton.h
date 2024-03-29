#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H
#include<QDebug>
#include<QWidget>
#include<QPushButton>
#include<QMouseEvent>
class MyPushButton : public QPushButton
{
    Q_OBJECT
public:
    //explicit MyPushButton(QWidget *parent = 0);
    //重写构造函数 normalImg按钮按下前的图片路径 pressImg按钮按下后的图片路径
    MyPushButton(QString normalImg,QString pressImg="");
    //重写按钮 按下和释放事件
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void KillAction(int x,int y);
    void zoom1();//向下跳
    void zoom2();//向上跳
signals:

public slots:
private:
    QString normalImgPath;//按钮按下前的图片路径
    QString pressImgPath;//按钮按下后的图片路径
};

#endif // MYPUSHBUTTON_H
