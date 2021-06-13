#ifndef HUATUO_H
#define HUATUO_H
#include"hero.h"

class HuaTuo:public Hero
{
public:
    HuaTuo();
    HuaTuo(int identity);
    virtual ~HuaTuo();
    void FirstSkill();
    void SecondSkill();
};

#endif // HUATUO_H
