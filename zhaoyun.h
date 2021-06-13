#ifndef ZHAOYUN_H
#define ZHAOYUN_H
#include"hero.h"

class ZhaoYun:public Hero
{
public:
    ZhaoYun();
    ZhaoYun(int identity);
    void FirstSkill();
    void SecondSkill();
    virtual ~ZhaoYun();
};

#endif // ZHAOYUN_H
