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