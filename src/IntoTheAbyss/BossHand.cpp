#include "BossHand.h"
#include"../Engine/DrawFunc.h"
#include"../IntoTheAbyss/TexHandleMgr.h"
#include"../Common/KuroMath.h"

BossHand::BossHand(int HANDLE) :graphHandle(HANDLE), radian(0.0f), centralPos({ 0.0f,0.0f }), pos({ 0.0f,0.0f }), size({ 1.0f,1.0f })
{
}

void BossHand::Init()
{
}

void BossHand::Update(const Vec2<float> &POS, float RADIUS, float ANGLE)
{
	centralPos = POS;
	radian = Angle::ConvertToRadian(ANGLE);
	pos = centralPos + Vec2<float>(cosf(radian) * RADIUS, sinf(radian) * RADIUS);
}

void BossHand::Draw()
{
	DrawFunc::DrawRotaGraph2D(pos, size, radian, TexHandleMgr::GetTexBuffer(graphHandle));
	DrawFunc::DrawCircle2D(centralPos, 1.0f, Color(255, 255, 255, 255));
}
