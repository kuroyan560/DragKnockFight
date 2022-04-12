#pragma once
#include"../Common/Vec.h"

class MiniMap
{
public:
	MiniMap();

	void CalucurateCurrentPos(const Vec2<float> &POS);
	void Init(const float &MAX_VALUE);
	void Update();
	void Draw();

	float miniX;
	float massX;
private:
	Vec2<float>	nowPos;
	float maxValue;
	float nowValue;
	float keep;

	int lineHandle;
	int nowPosHandle;

	int leftWall;
	int rightWall;
};

