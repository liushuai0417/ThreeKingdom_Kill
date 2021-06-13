#ifndef ZHANGFEI_H
#define ZHANGFEI_H
#include"hero.h"

class ZhangFei:public Hero
{
public:
    ZhangFei();
    ZhangFei(int identity);
    void FirstSkill();
    void SecondSkill();
    virtual ~ZhangFei();
};

#endif // ZHANGFEI_H
