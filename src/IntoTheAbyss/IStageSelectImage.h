#pragma once
#include"../KuroEngine.h"

class IStageSelectImage
{
protected:
	Vec2<float>pos, size;
	Vec2<float>lerpPos, lerpSize;

	void Lerp(const Vec2<float> &POS, const Vec2<float> &SIZE)
	{
		lerpPos = POS;
		lerpSize = SIZE;

		Vec2<float>distance = lerpPos - pos;
		pos += distance * 0.1f;
		Vec2<float>distance2 = lerpSize - size;
		size += distance2 * 0.1f;
	};
};

