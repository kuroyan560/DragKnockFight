#include "ThornBlock.h"
#include"EventCollider.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"Collider.h"

Vec2<float> ThornBlock::adjValue = { 0.0f,0.0f };

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

bool ThornBlock::HitBox(Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE, Vec2<float> &PLAYER_VEL, Vec2<float> &PLAYER_PREV_POS)
{
	Vec2<float> hitBoxSize = (size + adjValue) / 2.0f;
	Vec2<float> hitBoxCentralPos = (leftUpPos + hitBoxSize);

	bool isDossunVel = EventCpllider::Instance()->CheckHitVel(PLAYER_POS, PLAYER_PREV_POS, PLAYER_VEL, SIZE, hitBoxCentralPos, hitBoxSize) != INTERSECTED_NONE;
	bool topHitFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, SIZE, hitBoxCentralPos, hitBoxSize, INTERSECTED_TOP) != INTERSECTED_NONE;
	bool buttomFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, SIZE, hitBoxCentralPos, hitBoxSize, INTERSECTED_BOTTOM) != INTERSECTED_NONE;
	bool leftHitFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, SIZE, hitBoxCentralPos, hitBoxSize, INTERSECTED_LEFT) != INTERSECTED_NONE;
	bool rightHitFlag = EventCpllider::Instance()->CheckHitSize(PLAYER_POS, SIZE, hitBoxCentralPos, hitBoxSize, INTERSECTED_RIGHT) != INTERSECTED_NONE;

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
	Vec2<float> drawLeftUpPos = leftUpPos - adjValue;
	Vec2<float> drawRightDownPos = (leftUpPos + size) + adjValue;
	drawLeftUpPos -= scrollAmount;
	drawRightDownPos -= scrollAmount;

	//“–‚½‚è”»’è•`‰æ—p
	DrawFunc::DrawBox2D(drawLeftUpPos, drawRightDownPos, color, DXGI_FORMAT_R8G8B8A8_UNORM);
}
