#pragma once
#include"../Common/Vec.h"
#include"ScreenEdge.h"
#include<array>

class ScreenEdgeEffect
{
public:
	ScreenEdgeEffect();
	void Init();
	void Update();
	void Draw();

	void CheckPos(const float &LINE_CENTER_POS);

	bool leftWinFlag;
	bool rightWinFlag;

	void LeftPlayerWin();
	void RightPlayerWin();
private:
	Vec2<float>pos;
	float nowDistance;


	enum 
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
	};
	std::array<ScreenEdge, 4>screenEdge;
};

