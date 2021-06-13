#include "zhangfei.h"

ZhangFei::ZhangFei()
{

}

ZhangFei::ZhangFei(int identity){
    //如果是主公
    this->m_nblood = 5;
    //如果是其他身份
    this->m_nblood = 4;
    this->m_nidentity = identity;
    this->m_ncount = 1;
}

void ZhangFei::FirstSkill(){
    //出杀无限制
}

void ZhangFei::SecondSkill(){
    //空实现
}
