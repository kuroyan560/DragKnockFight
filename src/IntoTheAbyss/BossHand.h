#pragma once
#include"../Common/Vec.h"

class BossHand
{
public:
	BossHand(int HANDLE, int HOLD_HANDLE);
	
	void Init();
	void Update(const Vec2<float> &POS, float RADIUS, float ANGLE, bool HOLD);
	void Draw();

private:
	int nowHnadle,graphHandle, holdGraphHandle;
	float radian;
	Vec2<float> pos, size, centralPos;
};
