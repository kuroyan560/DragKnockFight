#pragma once
#include"../Common/Vec.h"

class BossHand
{
public:
	BossHand(int HANDLE);
	
	void Init();
	void Update(const Vec2<float> &POS, float RADIUS, float ANGLE);
	void Draw();

private:
	int graphHandle;
	float radian;
	Vec2<float> pos, size, centralPos;
};
