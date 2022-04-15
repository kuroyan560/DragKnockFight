#include "BossPatternSwing.h"

BossPatternSwing::BossPatternSwing()
{
}

void BossPatternSwing::Init()
{
	swingFlag = false;
	initSwingFlag = false;
}

void BossPatternSwing::Update(BossPatternData *DATA)
{
	//一回のみスウィングを行う
	swingFlag = false;
	if (!initSwingFlag)
	{
		swingFlag = true;
	}
}
