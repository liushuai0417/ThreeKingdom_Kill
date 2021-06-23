#include "herobutton.h"
#include<QPixmap>
#include<QPropertyAnimation>
#include<QDebug>
//HeroButton::HeroButton(QWidget *parent) : QWidget(parent)
//{

//}

HeroButton::HeroButton(QString normalImg,QString pressImg){
    b_flagchoose = false;//默认是非选中状态
    this->normalImgPath = normalImg;
    this->pressImgPath = pressImg;
    //加载图片
    QPixmap pix;
    bool ret = pix.load(normalImg);
    if(!ret){
        qDebug()<<__func__<<"图片加载失败";
        return;
    }

    //设置图片固定大小
    this->setFixedSize(pix.width(),pix.height());
    //设置不规则图片格式
    this->setStyleSheet("QPushButton{border:0px;}");//边框设置为0像素
    //设置图标
    this->setIcon(pix);
    //设置图标大小
    this->setIconSize(QSize(pix.width(),pix.height()));
}

//重写按钮 按下和释放事件
void HeroButton::mousePressEvent(QMouseEvent *e){
    //传入的按下图片不为空 证明有需要按下的状态 切换图片
    if(this->pressImgPath != ""){
        QPixmap pix;
        //加载图片
        bool res = pix.load(this->pressImgPath);
        if(!res){
            qDebug()<<__func__<<"图片加载失败";
            return;
        }

        //设置图片固定大小
        this->setFixedSize(pix.width(),pix.height());
        //设置不规则图片格式
        this->setStyleSheet("QPushButton{border:0px;}");//边框0像素
        //设置图标
        this->setIcon(pix);
        //设置图标大小
        this->setIconSize(QSize(pix.width(),pix.height()));
    }
    //让父类执行其他内容
    return QPushButton::mousePressEvent(e);
}

void HeroButton::mouseReleaseEvent(QMouseEvent *e){
    //传入的按下图片不为空 证明有需要按下的状态 切换成原始的图片
    if(this->pressImgPath != ""){
        QPixmap pix;
        //加载图片
        bool res = pix.load(this->normalImgPath);
        if(!res){
            qDebug()<<__func__<<"图片加载失败";
            return;
        }

        //设置图片固定大小
        this->setFixedSize(pix.width(),pix.height());
        //设置不规则图片格式
        this->setStyleSheet("QPushButton{border:0px;}");//边框0像素
        //设置图标
        this->setIcon(pix);
        //设置图标大小
        this->setIconSize(QSize(pix.width(),pix.height()));
    }
    //让父类执行其他内容
    return QPushButton::mouseReleaseEvent(e);
}

//向下跳
void HeroButton::zoom1(){
    //创建动画对象
    QPropertyAnimation *animation = new QPropertyAnimation(this,"geometry");
    //设置动画时间间隔
    //设置起始位置
    animation->setStartValue(QRect(this->x(),this->y(),this->width(),this->height()));
    //结束位置
    animation->setEndValue(QRect(this->x(),this->y()+30,this->width(),this->height()));
    //设置弹跳曲线
    animation->setEasingCurve(QEasingCurve::OutBounce);
    //执行动画
    animation->start();
}

//向上跳
void HeroButton::zoom2(){
    //创建动画对象
    QPropertyAnimation *animation = new QPropertyAnimation(this,"geometry");
    //设置动画时间间隔

    //设置起始位置
    animation->setStartValue(QRect(this->x(),this->y()+30,this->width(),this->height()));
    //结束位置
    animation->setEndValue(QRect(this->x(),this->y(),this->width(),this->height()));
    //设置弹跳曲线
    animation->setEasingCurve(QEasingCurve::OutBounce);
    //执行动画
    animation->start();
}

void HeroButton::ChooseHero(){
    //创建动画对象
    QPropertyAnimation *animation = new QPropertyAnimation(this,"geometry");
    //设置动画时间间隔

    //设置起始位置
    animation->setStartValue(QRect(this->x(),this->y(),this->width(),this->height()));
    //结束位置
    animation->setEndValue(QRect(this->x(),this->y()-30,this->width(),this->height()));
    //设置弹跳曲线
    animation->setEasingCurve(QEasingCurve::OutBounce);
    //执行动画
    animation->start();
}

void HeroButton::ChooseHero1(){
    //创建动画对象
    QPropertyAnimation *animation = new QPropertyAnimation(this,"geometry");
    //设置动画时间间隔

    //设置起始位置
    animation->setStartValue(QRect(this->x(),this->y(),this->width(),this->height()));
    //结束位置
    animation->setEndValue(QRect(this->x(),this->y()+30,this->width(),this->height()));
    //设置弹跳曲线
    animation->setEasingCurve(QEasingCurve::OutBounce);
    //执行动画
    animation->start();
}
