#ifndef HERO_H
#define HERO_H


class Hero
{
public:
    Hero();
    Hero(int identity);
    ~Hero();
public:
    int m_nblood;//血量
    int m_nidentity;//身份
    int m_ncount;//技能数
    int m_ncard;//手牌数
public:
    virtual void FirstSkill() = 0;
    virtual void SecondSkill() = 0;
};

#endif // HERO_H
