#pragma once
#include"../KuroEngine.h"

class BossHand
{
public:
	BossHand();

	void Init();
	void Update(const Vec2<float> &POS, float RADIUS, float RADIAN);
	void Draw();

private:
	Vec2<float>centralPos, pos;
	float inputAngle;
	int handGraph;
};

