#include "BossHand.h"
#include"../Engine/DrawFunc.h"
#include"../IntoTheAbyss/TexHandleMgr.h"
#include"../Common/KuroMath.h"
#include"../IntoTheAbyss/ScrollMgr.h"
#include"../IntoTheAbyss/CharacterAIData.h"
#include"../IntoTheAbyss/CharacterManager.h"

BossHand::BossHand(int HANDLE, int HOLD_HANDLE, bool SHAKE_FLAG) :graphHandle(HANDLE), holdGraphHandle(HOLD_HANDLE),
radian(0.0f), centralPos({ 0.0f,0.0f }), pos({ 0.0f,0.0f }), size({ 1.0f,1.0f }), shakeFlag(SHAKE_FLAG)
{
	count = 0;
	nowHnadle = graphHandle;
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
		angle = 0.0f;
		bossCount = 0.0f;
	}

	centralPos = POS;
	//radian = Angle::ConvertToRadian(ANGLE);
	radian = ANGLE;

	
	Vec2<float> shakeAmount = { 0.0f,0.0f };
	if (shakeFlag)
	{
		Vec2<float> buff =  CharacterManager::Instance()->Right()->shakeValue;
		shakeAmount = CharacterManager::Instance()->Right()->shakeValue;
	}
	pos = centralPos + Vec2<float>((cosf(radian) * RADIUS) + shakeAmount.x, (sinf(radian) * RADIUS) + shakeAmount.y);
}

void BossHand::Draw(bool SCROL_ON)
{
	if (SCROL_ON)
	{
		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), size * ScrollMgr::Instance()->zoom * CharacterManager::Instance()->Left()->GetAlphaRate(), radian, TexHandleMgr::GetTexBuffer(nowHnadle));
	}
	else
	{
		DrawFunc::DrawRotaGraph2D(pos, size, radian, TexHandleMgr::GetTexBuffer(nowHnadle));
	}
}
