#pragma once
#include"BossHand.h"
#include<memory>

struct HandData
{
	Vec2<float>pos;
	float radius;
	float angle;
};

class BossHandMgr
{
public:
	BossHandMgr();
	void Init();
	void Update();
	void Draw();

	void ImGuiDraw();
	std::unique_ptr<BossHand>leftHand, rightHand;

	HandData leftHandData, rightHandData;
};

