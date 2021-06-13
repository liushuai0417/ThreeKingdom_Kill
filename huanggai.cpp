#include "huanggai.h"

HuangGai::HuangGai()
{

}

HuangGai::HuangGai(int identity){
    //如果是主公
    this->m_nblood = 5;
    //如果是其他身份
    this->m_nblood = 4;
    this->m_nidentity = identity;
    this->m_ncount = 1;
}

void HuangGai::FirstSkill(){
    //出牌阶段

    //blood-1

    //服务器发两张牌
}

void HuangGai::SecondSkill(){
    //空实现
}
