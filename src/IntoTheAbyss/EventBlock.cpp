#include "EventBlock.h"

EventBlock::EventBlock() :initFlag(false)
{
}

void EventBlock::Init(const Vec2<float> &POS)
{
	pos = POS;
	initFlag = true;
}

void EventBlock::Finalize()
{
	initFlag = false;
}

bool EventBlock::HitBox(const Vec2<float> &PLAYER_POS)
{
	//���������Ă��Ȃ��u���b�N�͓����蔻��͕Ԃ��Ȃ�
	if (initFlag)
	{
		return false;
	}


	return false;
}
