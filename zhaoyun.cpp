#include "zhaoyun.h"

ZhaoYun::ZhaoYun()
{

}

ZhaoYun::~ZhaoYun()
{

}

ZhaoYun::ZhaoYun(int identity){
    //如果是主公
    this->m_nblood = 5;
    //如果是其他身份
    this->m_nblood = 4;
    this->m_nidentity = identity;
    this->m_ncount = 1;
}

void ZhaoYun::FirstSkill(){
    //杀当闪 闪当杀
}

void ZhaoYun::SecondSkill(){

}
