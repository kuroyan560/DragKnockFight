#include "BossPatternNormalMove.h"
#include "Camera.h"
#include"../KuroEngine.h"

BossPatternNormalMove::BossPatternNormalMove()
{
}

void BossPatternNormalMove::Init()
{
}

void BossPatternNormalMove::Update(BossPatternData *DATA)
{
	if (Camera::Instance()->Active())
	{
		*DATA->bossPos = { 0,0 };
		return;
	}

	static const int PULL_SPAN_MIN = 30;
	static const int PULL_SPAN_MAX = 70;
	static int PULL_SPAN = KuroFunc::GetRand(PULL_SPAN_MIN, PULL_SPAN_MAX);
	static int PULL_TIMER = 0;
	static Vec2<float>ACCEL = { 0.0f,0.0f };	//‰Á‘¬“x
	static const float PULL_POWER_MIN = 15.0f;
	static const float PULL_POWER_MAX = 25.0f;

	if (PULL_TIMER < PULL_SPAN)
	{
		PULL_TIMER++;
		if (PULL_SPAN <= PULL_TIMER)
		{
			PULL_SPAN = KuroFunc::GetRand(PULL_SPAN_MIN, PULL_SPAN_MAX);
			PULL_TIMER = 0;

			auto rad = Angle::ConvertToRadian(KuroFunc::GetRand(-70, 70));
			auto power = KuroFunc::GetRand(PULL_POWER_MIN, PULL_POWER_MAX);
			ACCEL.x = cos(rad) * power * 1.6f;
			ACCEL.y = sin(rad) * power;
		}
	}
	DATA->bossPos->x = KuroMath::Lerp(DATA->bossPos->x, 10.0f, 0.1f);
	DATA->bossPos->y = KuroMath::Lerp(DATA->bossPos->y, 0.0f, 0.1f);
	*DATA->bossPos += ACCEL;

	ACCEL = KuroMath::Lerp(ACCEL, { 0.0f,0.0f }, 0.8f);

	if (UsersInput::Instance()->Input(DIK_0)) {
	//	vel.x = OFFSET_VEL * 5.0f;
		bool debug = false;
	}
}
