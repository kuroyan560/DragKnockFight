#include "OperatingLayer.h"
#include"CharacterManager.h"

OperateMove::OperateMove()
{
	initFlag = false;
}

void OperateMove::Update(const Vec2<float> &VELOCITY)
{
	//�ړ�
	if (!initFlag)
	{
		oldPos = CharacterManager::Instance()->Right()->pos;
		initFlag = true;
	}
	CharacterAIData::Instance()->vel = VELOCITY;
	oldPos = CharacterManager::Instance()->Right()->pos;
}

AiResult OperateMove::CurrentProgress()
{
	//���W�������Ă����琬���A�~�܂��Ă����玸�s
	if (CharacterManager::Instance()->Right()->pos != oldPos)
	{
		return AiResult::OPERATE_SUCCESS;
	}
	else
	{
		return AiResult::OPERATE_FAIL;
	}
}