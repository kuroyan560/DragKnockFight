#pragma once
#include"../Common/Vec.h"

class BossHand
{
public:
	BossHand(int HANDLE);

	void Init();
	void Update(const Vec2<float> &POS, float RADIUS, float RADIAN);
	void Draw();

private:
	Vec2<float>centralPos, pos;
	float inputAngle;
	int handGraph;
};

