#include "OperatingLayer.h"
#include"CharacterManager.h"

OperateMove::OperateMove()
{
	initFlag = false;
}

void OperateMove::Update(const Vec2<float> &VELOCITY)
{
	//ˆÚ“®
	CharacterAIOrder::Instance()->vel = VELOCITY;
}

AiResult OperateMove::CurrentProgress()
{
	bool succeedFlag = false;
	//À•W‚ª“®‚¢‚Ä‚¢‚½‚ç¬Œ÷AŽ~‚Ü‚Á‚Ä‚¢‚½‚çŽ¸”s
	if (CharacterManager::Instance()->Right()->pos != oldPos)
	{
		succeedFlag = true;
	}
	else
	{
		succeedFlag = false;
	}
	oldPos = CharacterManager::Instance()->Right()->pos;

	if (succeedFlag)
	{
		return AiResult::OPERATE_SUCCESS;
	}
	else
	{
		return AiResult::OPERATE_FAIL;
	}
}