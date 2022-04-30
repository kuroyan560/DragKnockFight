#include "OperatingLayer.h"

OperateMove::OperateMove(const std::shared_ptr<Vec2<float>> &POS)
{
	pos = POS;
	oldPos = *pos;
}

void OperateMove::Update(const Vec2<float> &VELOCITY)
{
	//�ړ�
	oldPos = *pos;
	*pos += VELOCITY;
}

AiResult OperateMove::CurrentProgress()
{
	//���W�������Ă����琬���A�~�܂��Ă����玸�s
	if (*pos != oldPos)
	{
		return AiResult::OPERATE_SUCCESS;
	}
	else
	{
		return AiResult::OPERATE_FAIL;
	}
}