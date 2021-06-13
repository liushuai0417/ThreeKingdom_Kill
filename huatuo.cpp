#include "huatuo.h"

HuaTuo::HuaTuo()
{

}

HuaTuo::~HuaTuo(){

}

HuaTuo::HuaTuo(int identity){
    //如果是主公
    this->m_nblood = 4;
    //如果是其他身份
    this->m_nblood = 3;
    this->m_nidentity = identity;
    this->m_ncount = 2;
}

void HuaTuo::FirstSkill(){
    //出牌阶段

    //红桃/方片当做桃
}

void HuaTuo::SecondSkill(){
    //弃用一张手牌

    //blood+1
}
