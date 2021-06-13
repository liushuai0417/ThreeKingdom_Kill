#include "zhangliao.h"

ZhangLiao::ZhangLiao()
{

}

ZhangLiao::ZhangLiao(int identity){
    //如果是主公
    this->m_nblood = 5;
    //如果是其他身份
    this->m_nblood = 4;
    this->m_nidentity = identity;
    this->m_ncount = 1;
}

void ZhangLiao::FirstSkill(){
    //放弃摸牌

    //选择角色抽牌
}

void ZhangLiao::SecondSkill(){
    //空实现
}
