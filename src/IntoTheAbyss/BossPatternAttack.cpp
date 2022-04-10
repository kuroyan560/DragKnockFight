#include "BossPatternAttack.h"
#include"../Common/KuroMath.h"

BossPatternAttack::BossPatternAttack()
{
}

void BossPatternAttack::Init()
{
	shotInterval = 0;
}

void BossPatternAttack::Update(BossPatternData *DATA)
{
	++shotInterval;
	if (shotInterval % 60 == 0)
	{
		for (int i = 0; i < 20; ++i)
		{
			DATA->bulltData[i].initFlag = true;
			float angle = i * (360.0f / 20.0f);
			DATA->bulltData[i].dir = angle * Angle::PI() / 180.0f;
			DATA->bulltData[i].speed = 15.0f;
		}
		shotInterval = 0;
	}
}
