#include "CrashInterFace.h"
#include"KuroMath.h"
#include"KuroFunc.h"

void CrashInterFace::Init()
{
	shake = { 0,0 };
	flashAlpha = 0.0f;
	timer = CRASH_SPAN + 1;
	sign = { KuroFunc::GetRandPlusMinus() ,KuroFunc::GetRandPlusMinus() };
}

void CrashInterFace::Update()
{
	if (CRASH_SPAN < timer)return;

	if (timer % SHAKE_SPAN == 0)
	{
		float power = KuroMath::Ease(Out, Circ, timer, CRASH_SPAN, 1.0f, 0.0f) * SHAKE_FULL_POWER;
		shake.x = power * sign.x;
		shake.y = power * sign.y;
		sign *= -1;
	}
	flashAlpha = KuroMath::Ease(Out, Circ, timer, CRASH_SPAN, 0.7f, 0.0f);

	timer++;
}

void CrashInterFace::Crash()
{
	timer = 0;
}