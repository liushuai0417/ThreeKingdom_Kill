#ifndef GANNING_H
#define GANNING_H
#include"hero.h"

class GanNing:public Hero
{
public:
    GanNing();
    GanNing(int identity);
    virtual ~GanNing();
    void FirstSkill();
    void SecondSkill();
};

#endif // GANNING_H
