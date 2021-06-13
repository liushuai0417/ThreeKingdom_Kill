#ifndef LVBU_H
#define LVBU_H

#include"hero.h"
class LvBu:public Hero
{
public:
    LvBu();
    LvBu(int identity);
    void FirstSkill();
    void SecondSkill();
    virtual ~LvBu();
};

#endif // LVBU_H
