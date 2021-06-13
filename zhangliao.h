#ifndef ZHANGLIAO_H
#define ZHANGLIAO_H
#include"hero.h"

class ZhangLiao:public Hero
{
public:
    ZhangLiao();
    ZhangLiao(int identity);
    void FirstSkill();
    void SecondSkill();
    virtual ~ZhangLiao();
};

#endif // ZHANGLIAO_H
