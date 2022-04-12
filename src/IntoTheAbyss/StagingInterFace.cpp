#include "StagingInterFace.h"
#include"KuroMath.h"
#include"KuroFunc.h"

void StagingInterFace::Init()
{
	shakeTimer = -1;
	shake = { 0,0 };

	extTimer = -1;
	extRate = { 1.0f,1.0f };

	flashTimer = -1;
	flashAlpha = 0.0f;
}

void StagingInterFace::Update()
{
	//êUìÆ
	if (0 < shakeTimer)
	{
		int timer = shakeTotalTime - shakeTimer;
		if (timer % shakeSpan == 0)
		{
			float power = KuroMath::Ease(Out, Circ, timer, shakeTotalTime, 1.0f, 0.0f) * shakePower;
			shake = sign.Float() * power;
			sign *= -1;
		}
		shakeTimer--;
	}

	//êLèk
	if (0 < extTimer)
	{
		int timer = extTotalTime - extTimer;
		extRate = KuroMath::Ease(In, Back, timer, extTotalTime, Vec2<float>(1.0f, 1.0f) + addExtRate, { 1.0f,1.0f });
		extTimer--;
	}

	//ì_ñ≈
	if (0 < flashTimer)
	{
		int timer = flashTotalTime - flashTimer;
		flashAlpha = KuroMath::Ease(Out, Circ, timer, flashTotalTime, flashRate, 0.0f);
		flashTimer--;
	}
}

void StagingInterFace::Shake(const int& Time, const int& Span, const int& Power)
{
	shakeTimer = Time;
	shakeTotalTime = Time;
	shakeSpan = Span;
	shakePower = Power;
	sign = { KuroFunc::GetRandPlusMinus() ,KuroFunc::GetRandPlusMinus() };
}

void StagingInterFace::Extend(const int& Time, const Vec2<bool>& Ext, const float& ExtRateAmount)
{
	extTimer = Time;
	extTotalTime = Time;
	if (Ext.x && Ext.y)addExtRate = { ExtRateAmount,ExtRateAmount };
	else if (Ext.x)addExtRate = { ExtRateAmount,-ExtRateAmount };
	else if (Ext.y)addExtRate = { -ExtRateAmount,ExtRateAmount };
}

void StagingInterFace::Flash(const int& Time, const float& FlashRate)
{
	flashTimer = Time;
	flashTotalTime = Time;
	flashRate = FlashRate;
}
