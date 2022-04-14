#include "ThornBlock.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"Collider.h"
#include"GimmickLoader.h"

Vec2<float> ThornBlock::adjValue = { 0.0f,0.0f };

ThornBlock::ThornBlock()
{
	adjValue = GimmickLoader::Instance()->thownData->adjValue;
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

	color = Color(255, 255, 255, 255);
	return false;
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
