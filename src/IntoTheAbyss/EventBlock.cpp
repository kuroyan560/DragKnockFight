#include "EventBlock.h"
#include"StageMgr.h"
#include"EventCollider.h"
#include"DrawFunc.h"
#include"TexHandleMgr.h"

EventBlock::EventBlock() :chipNumber(-1)
{
}

void EventBlock::Init(const Vec2<float> &LEFT_UP_POS, const Vec2<float> &RIGHT_DOWN_POS, const int &DOOR_CHIP_NUM)
{
	leftUpPos = LEFT_UP_POS;
	size = RIGHT_DOWN_POS - leftUpPos;
	chipNumber = DOOR_CHIP_NUM;
}

bool EventBlock::HitBox(const Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE, const Vec2<float> &PLAYER_VEL, const Vec2<float> &PLAYER_PREV_POS)
{
	Vec2<float> hitBoxSize = size / 2.0f;
	Vec2<float> hitBoxCentralPos = (leftUpPos + hitBoxSize);

	bool isDossunVel = EventCpllider::Instance()->CheckHitVel(PLAYER_POS, PLAYER_PREV_POS, PLAYER_VEL, SIZE, hitBoxCentralPos, hitBoxSize) != INTERSECTED_NONE;
	bool topHitFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, SIZE, hitBoxCentralPos, hitBoxSize, INTERSECTED_TOP) != INTERSECTED_NONE;
	bool buttomFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, SIZE, hitBoxCentralPos, hitBoxSize, INTERSECTED_BOTTOM) != INTERSECTED_NONE;
	bool leftHitFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, SIZE, hitBoxCentralPos, hitBoxSize, INTERSECTED_LEFT) != INTERSECTED_NONE;
	bool rightHitFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, SIZE, hitBoxCentralPos, hitBoxSize, INTERSECTED_RIGHT) != INTERSECTED_NONE;

	if (isDossunVel || topHitFlag || buttomFlag || leftHitFlag || rightHitFlag)
	{
		return true;
	}
	else
	{
		return false;
	}
}