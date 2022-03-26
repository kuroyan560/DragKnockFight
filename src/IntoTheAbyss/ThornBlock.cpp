#include "ThornBlock.h"
#include"EventCollider.h"
#include"../Engine/DrawFunc.h"

ThornBlock::ThornBlock()
{
}

void ThornBlock::Init(const Vec2<float> &LEFT_UP_POS, const Vec2<float> &RIGHT_DOWN_POS)
{
	leftUpPos = LEFT_UP_POS;
	size = RIGHT_DOWN_POS - leftUpPos;
}

void ThornBlock::Finalize()
{
}

bool ThornBlock::HitBox(const Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE, const Vec2<float> &PLAYER_VEL, const Vec2<float> &PLAYER_PREV_POS)
{
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

void ThornBlock::Draw()
{
	//“–‚½‚è”»’è•`‰æ—p
	DrawFunc::DrawBox2D(leftUpPos, leftUpPos + size, Color(255, 0, 0, 255), DXGI_FORMAT_R8G8B8A8_UNORM);
}
