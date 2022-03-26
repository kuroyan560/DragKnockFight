#include "ThornBlock.h"
#include"EventCollider.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"

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
	Vec2<float> hitBoxLeftUpPos = leftUpPos;
	Vec2<float> hitBoxRightDownPos = (leftUpPos + size);

	bool isDossunVel = EventCpllider::Instance()->CheckHitVel(PLAYER_POS, PLAYER_PREV_POS, PLAYER_VEL, SIZE, leftUpPos, size) != INTERSECTED_NONE;
	bool topHitFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, size, leftUpPos, SIZE, INTERSECTED_TOP);
	bool buttomFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, size, leftUpPos, SIZE, INTERSECTED_BOTTOM);
	bool leftHitFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, size, leftUpPos , SIZE, INTERSECTED_LEFT);
	bool rightHitFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, size, leftUpPos, SIZE, INTERSECTED_RIGHT);

	if (isDossunVel || topHitFlag || buttomFlag || leftHitFlag || rightHitFlag)
	{
		color = Color(255, 0, 0, 255);
		return true;
	}
	else
	{
		color = Color(255, 255, 255, 255);
		return false;
	}
}

void ThornBlock::Draw()
{
	Vec2<float>scrollAmount = ScrollMgr::Instance()->scrollAmount;
	Vec2<float> drawLeftUpPos = leftUpPos - scrollAmount;
	Vec2<float> drawRightDownPos = (leftUpPos + size) - scrollAmount;
	drawLeftUpPos += adjValue;
	drawRightDownPos += adjValue;

	//“–‚½‚è”»’è•`‰æ—p
	DrawFunc::DrawBox2D(drawLeftUpPos, drawRightDownPos, color, DXGI_FORMAT_R8G8B8A8_UNORM);
}
