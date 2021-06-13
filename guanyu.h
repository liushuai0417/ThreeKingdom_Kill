#ifndef GUANYU_H
#define GUANYU_H
#include"hero.h"

class GuanYu:public Hero
{
public:
    GuanYu();
    GuanYu(int identity);
    virtual ~GuanYu();
    void FirstSkill();
    void SecondSkill();
};

#endif // GUANYU_H
