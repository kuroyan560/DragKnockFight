#pragma once
#include"../KuroEngine.h"

class Gauge
{
public:
	Gauge();

	void Init(int RATE_MAX);
	void Update(const Vec2<float> &POS);
	void Draw();

	bool canUseGague();
	void Use();
private:
	int rateMax;
	int nowRate;

	Vec2<float>gaugePos, gaugeSize;
	Vec2<float>leftVarPos, rightVarPos;

	int gaugeHandle;
	int varHandle;
};
