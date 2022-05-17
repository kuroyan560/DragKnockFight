#pragma once
#include"BossHand.h"
#include<memory>

class BossHandMgr
{
public:
	BossHandMgr();

	void Init();
	void Update();
	void Draw();

	std::unique_ptr<BossHand> leftHand, rightHande;
	int k = 0;
};

