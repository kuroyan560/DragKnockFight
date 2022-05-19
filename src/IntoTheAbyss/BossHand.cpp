#include "BossHand.h"
#include"../Engine/DrawFunc.h"
#include"../IntoTheAbyss/TexHandleMgr.h"
#include"../Common/KuroMath.h"
#include"../IntoTheAbyss/ScrollMgr.h"
#include"../IntoTheAbyss/CharacterAIData.h"

BossHand::BossHand(int HANDLE, int HOLD_HANDLE) :graphHandle(HANDLE), holdGraphHandle(HOLD_HANDLE),
radian(0.0f), centralPos({ 0.0f,0.0f }), pos({ 0.0f,0.0f }), size({ 1.0f,1.0f })
{
	count = 0;
}

void BossHand::Init()
{
}

void BossHand::Update(const Vec2<float> &POS, float RADIUS, float ANGLE, bool HOLD, const Vec2<float> &DIR)
{
	if (HOLD)
	{
		nowHnadle = holdGraphHandle;
	}
	else
	{
		nowHnadle = graphHandle;
	}

	centralPos = POS;
	//radian = Angle::ConvertToRadian(ANGLE);
	radian = ANGLE;

	const float PI2 = 3.14f;
	count += 1 + 50 * CharacterAIOrder::Instance()->prevRate;
	float countDown = sinf(PI2 / 120.0f * count) * 0.0f;

	Vec2<float>shakeVec = DIR;
	shakeVec.x = 1.0f - fabs(shakeVec.x);
	shakeVec.y = 1.0f - fabs(shakeVec.y);

	pos = centralPos + Vec2<float>((cosf(radian) * RADIUS) + countDown * (shakeVec.x), (sinf(radian) * RADIUS) + countDown * (shakeVec.y));
}

void BossHand::Draw(bool SCROL_ON)
{
	if (SCROL_ON)
	{
		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), size * ScrollMgr::Instance()->zoom, radian, TexHandleMgr::GetTexBuffer(nowHnadle));
	}
	else
	{
		DrawFunc::DrawRotaGraph2D(pos, size, radian, TexHandleMgr::GetTexBuffer(nowHnadle));
	}
}
