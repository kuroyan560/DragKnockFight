#include "OperatingLayer.h"
#include"CharacterManager.h"

OperateMove::OperateMove()
{
	initFlag = false;

}

void OperateMove::Init()
{
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

OperateSwing::OperateSwing()
{
}

void OperateSwing::Init(int SWING_COOL_TIME)
{
	swingCoolTime = SWING_COOL_TIME;
	swingTimer = 0;
	enableToSwingFlag = false;
}

AiResult OperateSwing::SwingClockWise()
{
	if (enableToSwingFlag && !CharacterManager::Instance()->Right()->GetNowBreak())
	{
		CharacterAIOrder::Instance()->swingClockWiseFlag = true;
		swingTimer = 0;
		return AiResult::OPERATE_SUCCESS;
	}
	return AiResult::OPERATE_FAIL;
}

AiResult OperateSwing::SwingCounterClockWise()
{
	if (enableToSwingFlag)
	{
		CharacterAIOrder::Instance()->swingCounterClockWiseFlag = true;
		swingTimer = 0;
		return AiResult::OPERATE_SUCCESS;
	}
	return AiResult::OPERATE_FAIL;
}

AiResult OperateSwing::SwingLongDisntnce()
{
	if (enableToSwingFlag)
	{
		if (CharacterAIData::Instance()->swingCounterClockwiseDistance < CharacterAIData::Instance()->swingClockwiseDistance)
		{
			CharacterAIOrder::Instance()->swingClockWiseFlag = true;
		}
		else
		{
			CharacterAIOrder::Instance()->swingCounterClockWiseFlag = true;
		}
		swingTimer = 0;
		return AiResult::OPERATE_SUCCESS;
	}
	return AiResult::OPERATE_FAIL;
}

void OperateSwing::Update()
{
	if (swingCoolTime <= swingTimer && !CharacterManager::Instance()->Right()->GetNowBreak() && !CharacterManager::Instance()->Right()->GetNowSwing())
	{
		enableToSwingFlag = true;
	}
	else
	{
		enableToSwingFlag = false;
	}
	++swingTimer;
}