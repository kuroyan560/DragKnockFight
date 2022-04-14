#include "BossPatternAttack.h"
#include"../Common/KuroMath.h"

BossPatternAttack::BossPatternAttack() :SHOT_BULLET_MAX(20.0f), MAX_SPEED(15.0f), SHOT_INTERVAL_MAX(60)
{
}

void BossPatternAttack::Init()
{
	shotInterval = 0;
	changeAngleFlag = true;
}

void BossPatternAttack::Update(BossPatternData *DATA)
{
	++shotInterval;
	if (shotInterval % SHOT_INTERVAL_MAX == 0)
	{
		if (changeAngleFlag)
		{
			for (int i = 0; i < SHOT_BULLET_MAX; ++i)
			{
				DATA->bulltData[i].initFlag = true;
				float angle = i * (360.0f / SHOT_BULLET_MAX);
				DATA->bulltData[i].dir = angle * Angle::PI() / 180.0f;
				DATA->bulltData[i].speed = MAX_SPEED;
			}
		}
		else
		{
			for (int i = 0; i < SHOT_BULLET_MAX; ++i)
			{
				DATA->bulltData[i].initFlag = true;
				float angle = (i * (360.0f / SHOT_BULLET_MAX)) + (360.0f / SHOT_BULLET_MAX) / 2.0f;
				DATA->bulltData[i].dir = angle * Angle::PI() / 180.0f;
				DATA->bulltData[i].speed = MAX_SPEED;
			}
		}

		changeAngleFlag = !changeAngleFlag;
		shotInterval = 0;
	}
}
