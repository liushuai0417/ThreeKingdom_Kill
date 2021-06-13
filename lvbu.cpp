#include "lvbu.h"

LvBu::LvBu()
{

}

LvBu::LvBu(int identity){
    //如果是主公
    this->m_nblood = 5;
    //如果是其他身份
    this->m_nblood = 4;
    this->m_nidentity = identity;
    this->m_ncount = 1;
}

void LvBu::FirstSkill(){
    //决斗 对方打两张杀

    //杀 对方打两张闪
}

void LvBu::SecondSkill(){
    //空实现
}
