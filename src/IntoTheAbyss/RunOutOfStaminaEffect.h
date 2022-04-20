#pragma once
#include"../Common/Vec.h"
#include"../IntoTheAbyss/ParticleMgr.h"
#include<memory>

class RunOutOfStaminaEffect
{
public:
	RunOutOfStaminaEffect();

	void Init();
	void Update();
	void Draw();

	void Start(const Vec2<float> &POS);
	bool isDraw();
private:

	Vec2<float> lerpEmptyStringPos;
	Vec2<float> baseEmptyStringPos;
	Vec2<float> emptyStringPos;
	int emptyHande;
	bool startFlag;
	int timer;
	bool drawFlag;
};

