#pragma once
#include"../KuroEngine.h"
#include"../Common/KuroMath.h"

struct LerpData
{
	Vec2<float>lerpPos, lerpSize;
	Vec2<float>pos, size;

	void Init(const Vec2<float> &POS, const Vec2<float> &SIZE)
	{
		lerpPos = POS;
		lerpSize = SIZE;

		pos = POS;
		size = SIZE;
	}

	void Lerp()
	{
		pos = KuroMath::Lerp(pos, lerpPos, 0.1f);
		size = KuroMath::Lerp(size, lerpSize, 0.1f);
	};
};