#include "BossPatternNormalMove.h"
#include "Camera.h"
#include"../KuroEngine.h"
#include"../IntoTheAbyss/DebugParameter.h"

BossPatternNormalMove::BossPatternNormalMove()
{
}

void BossPatternNormalMove::Init()
{
	PULL_TIMER = 0;
}

void BossPatternNormalMove::Update(BossPatternData *DATA)
{
	if (Camera::Instance()->Active())
	{
		*DATA->moveVel = { 0,0 };
		return;
	}

	int PULL_SPAN_MIN = DebugParameter::Instance()->bossDebugData.PULL_SPAN_MIN;
	int PULL_SPAN_MAX = DebugParameter::Instance()->bossDebugData.PULL_SPAN_MAX;
	int PULL_SPAN = KuroFunc::GetRand(PULL_SPAN_MIN, PULL_SPAN_MAX);
	
	Vec2<float>ACCEL = { 0.0f,0.0f };	//‰Á‘¬“x
	float PULL_POWER_MIN = DebugParameter::Instance()->bossDebugData.PULL_POWER_MIN;
	float PULL_POWER_MAX = DebugParameter::Instance()->bossDebugData.PULL_POWER_MAX;

	if (PULL_TIMER < PULL_SPAN)
	{
		PULL_TIMER++;
		if (PULL_SPAN <= PULL_TIMER)
		{
			PULL_SPAN = KuroFunc::GetRand(PULL_SPAN_MIN, PULL_SPAN_MAX);
			PULL_TIMER = 0;

			auto rad = Angle::ConvertToRadian(KuroFunc::GetRand(-70, 70));
			auto power = KuroFunc::GetRand(PULL_POWER_MIN, PULL_POWER_MAX);
			ACCEL.x = cos(rad) * power * DebugParameter::Instance()->bossDebugData.PULL_ADD_X_POWER;
			ACCEL.y = sin(rad) * power;
		}
	}
	DATA->moveVel->x = KuroMath::Lerp(DATA->moveVel->x, DebugParameter::Instance()->bossDebugData.moveX, 0.1f);
	DATA->moveVel->y = KuroMath::Lerp(DATA->moveVel->y, 0.0f, 0.1f);
	*DATA->moveVel += ACCEL;

	ACCEL = KuroMath::Lerp(ACCEL, { 0.0f,0.0f }, 0.8f);

	if (UsersInput::Instance()->Input(DIK_0)) {
	//	vel.x = OFFSET_VEL * 5.0f;
		bool debug = false;
	}
}
