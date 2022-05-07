#include "OperatingLayer.h"
#include"CharacterManager.h"

OperateMove::OperateMove()
{
	initFlag = false;
}

void OperateMove::Update(const Vec2<float> &VELOCITY)
{
	//�ړ�
	CharacterAIOrder::Instance()->vel = VELOCITY;

}

AiResult OperateMove::CurrentProgress()
{
	bool succeedFlag = false;
	//���W�������Ă����琬���A�~�܂��Ă����玸�s
	if (CharacterAIData::Instance()->nowPos != CharacterAIData::Instance()->prevPos)
	{
		succeedFlag = true;
	}
	else
	{
		succeedFlag = false;
	}

	if (succeedFlag)
	{
		return AiResult::OPERATE_SUCCESS;
	}
	else
	{
		return AiResult::OPERATE_FAIL;
	}
}