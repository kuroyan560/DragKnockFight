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
	//���̂݃X�E�B���O���s��
	swingFlag = false;
	if (!initSwingFlag)
	{
		swingFlag = true;
	}
}
