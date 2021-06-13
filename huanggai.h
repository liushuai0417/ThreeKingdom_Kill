#ifndef HUANGGAI_H
#define HUANGGAI_H
#include"hero.h"

class HuangGai:public Hero
{
public:
    HuangGai();
    virtual ~HuangGai();
    HuangGai(int identity);
    void FirstSkill();
    void SecondSkill();
};

#endif // HUANGGAI_H
