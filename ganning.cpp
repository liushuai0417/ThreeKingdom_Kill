#include "ganning.h"

GanNing::GanNing()
{
    //如果是主公 血量+1
}
GanNing::GanNing(int identity){
    //如果是主公
    this->m_nblood = 5;
    //如果是其他身份
    this->m_nblood = 4;
    this->m_nidentity = identity;
    this->m_ncount = 1;
}


void GanNing::FirstSkill(){
    //出牌阶段

    //牌如果是黑桃/梅花
    //相当于过河拆桥
}

void GanNing::SecondSkill(){
    //空实现
}
