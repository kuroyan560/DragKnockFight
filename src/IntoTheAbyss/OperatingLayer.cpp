#include "OperatingLayer.h"

OperateMove::OperateMove(const std::shared_ptr<Vec2<float>> &POS)
{
	pos = POS;
	oldPos = *pos;
}

void OperateMove::Update(const Vec2<float> &VELOCITY)
{
	//ˆÚ“®
	oldPos = *pos;
	*pos += VELOCITY;
	vel = oldPos - *pos;
}

AiResult OperateMove::CurrentProgress()
{
	//À•W‚ª“®‚¢‚Ä‚¢‚½‚ç¬Œ÷A~‚Ü‚Á‚Ä‚¢‚½‚ç¸”s
	if (*pos != oldPos)
	{
		return AiResult::OPERATE_SUCCESS;
	}
	else
	{
		return AiResult::OPERATE_FAIL;
	}
}