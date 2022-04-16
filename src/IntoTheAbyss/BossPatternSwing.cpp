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
	//���̂݃X�E�B���O���s��
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
