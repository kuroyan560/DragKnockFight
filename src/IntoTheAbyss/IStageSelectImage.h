#pragma once
#include"../KuroEngine.h"
#include"../Common/KuroMath.h"

struct LerpData
{
	Vec2<float>lerpPos, lerpSize;
	Vec2<float>startPos, startSize;
	Vec2<float>pos, size;
	float timer;

	void Init(const Vec2<float>& POS, const Vec2<float>& SIZE, const Vec2<float>& START_POS, const Vec2<float>& START_SIZE)
	{
		lerpPos = POS;
		lerpSize = SIZE;

		pos = POS;
		size = SIZE;

		timer = 0;
	}

	void EaseInOut()
	{
		timer += 0.02f;
		if (1.0f < timer) timer = 1.0f;

		pos = KuroMath::Ease(InOut, Exp, timer, startPos, lerpPos);
		size = KuroMath::Ease(InOut, Exp, timer, startSize, lerpSize);

		/*pos = KuroMath::Lerp(pos, lerpPos, 0.1f);
		size = KuroMath::Lerp(size, lerpSize, 0.1f);*/
	};

	void Lerp() {
		pos = KuroMath::Lerp(pos, lerpPos, 0.1f);
		size = KuroMath::Lerp(size, lerpSize, 0.1f);
	}

};