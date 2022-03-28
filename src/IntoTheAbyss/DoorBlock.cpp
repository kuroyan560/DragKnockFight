#include "DoorBlock.h"
#include"EventCollider.h"

DoorBlock::DoorBlock() :chipNumber(-1)
{
}

void DoorBlock::Init(const Vec2<float> &LEFT_UP_POS, const Vec2<float> &RIGHT_DOWN_POS, const int &DOOR_CHIP_NUM)
{
	leftUpPos = LEFT_UP_POS;
	size = RIGHT_DOWN_POS - leftUpPos;
	chipNumber = DOOR_CHIP_NUM;
}

void DoorBlock::Finalize()
{
	chipNumber = -1;
}

bool DoorBlock::Collision(const Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE, const Vec2<float> &PLAYER_VEL, const Vec2<float> &PLAYER_PREV_POS)
{
	//‰Šú‰»‚³‚ê‚Ä‚¢‚È‚¢‚È‚ç“–‚½‚è”»’è‚ð’Ê‚³‚È‚¢
	if (chipNumber == -1)
	{
		return false;
	}


	bool isDossunVel = EventCpllider::Instance()->CheckHitVel(PLAYER_POS, PLAYER_PREV_POS, PLAYER_VEL, SIZE, leftUpPos, size) != INTERSECTED_NONE;
	bool topHitFlag = EventCpllider::Instance()->CheckHitSize(leftUpPos, size, PLAYER_POS, SIZE, INTERSECTED_TOP);
	bool buttomFlag = EventCpllider::Instance()->CheckHitSize(leftUpPos, size, PLAYER_POS, SIZE, INTERSECTED_BOTTOM);
	bool leftHitFlag = EventCpllider::Instance()->CheckHitSize(leftUpPos, size, PLAYER_POS, SIZE, INTERSECTED_LEFT);
	bool rightHitFlag = EventCpllider::Instance()->CheckHitSize(leftUpPos, size, PLAYER_POS, SIZE, INTERSECTED_RIGHT);

	if (isDossunVel || topHitFlag || buttomFlag || leftHitFlag || rightHitFlag)
	{
		return true;
	}
	else
	{
		return false;
	}

}

