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

OperateDash::OperateDash()
{
	dashSpeed = { 0.0f,0.0f };
	rate = 1.0f;
	timer = 30.0f;
}

void OperateDash::Init(const Vec2<float> &DASH_SPEED)
{
	dashSpeed = DASH_SPEED;
	rate = 1.0f;
}

const Vec2<float> &OperateDash::Update()
{
	if (0.0f < rate)
	{
		rate += -(1.0f / timer);
	}
	else
	{
		rate = 0.0f;
	}
	return dashSpeed * rate;
}
