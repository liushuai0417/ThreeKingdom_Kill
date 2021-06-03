#include "mypushbutton.h"
#include<QPixmap>
//MyPushButton::MyPushButton(QWidget *parent) : QPushButton(parent)
//{

//}


MyPushButton::MyPushButton(QString normalImg,QString pressImg){
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
void MyPushButton::mousePressEvent(QMouseEvent *e){
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

void MyPushButton::mouseReleaseEvent(QMouseEvent *e){
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
