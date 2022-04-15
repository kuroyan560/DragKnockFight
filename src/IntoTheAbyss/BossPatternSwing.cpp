#include "BossPatternSwing.h"

BossPatternSwing::BossPatternSwing()
{
}

void BossPatternSwing::Init()
{
	initSwingFlag = false;
}

void BossPatternSwing::Update(BossPatternData *DATA)
{
	//一回のみスウィングを行う
	if (!initSwingFlag)
	{
		DATA->swingFlag = true;
		initSwingFlag = true;
	}
	else
	{
		DATA->swingFlag = false;
	}
}
