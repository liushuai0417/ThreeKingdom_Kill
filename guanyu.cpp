#include "guanyu.h"

GuanYu::GuanYu()
{

}

GuanYu::~GuanYu(){

}

GuanYu::GuanYu(int identity){
    //如果是主公
    this->m_nblood = 5;
    //如果是其他身份
    this->m_nblood = 4;
    this->m_nidentity = identity;
    this->m_ncount = 1;
}

void GuanYu::FirstSkill(){
    //如果是红桃/方片
    //出牌为杀
}

void GuanYu::SecondSkill(){
    //空实现
}
