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

	void Empty();
	void Full();
	//void Start(const Vec2<float> &POS, int MAX_TIEMR);
	Vec2<float>GetSize()
	{
		return playerSize;
	};
	Vec2<float>playerSize;
	Vec2<float> baseMaxStringPos;
	Vec2<float> baseEmptyStringPos;
private:
	Vec2<float> lerpEmptyStringPos;
	Vec2<float> emptyStringPos;

	Vec2<float> lerpMaxStringPos;
	Vec2<float> maxStringPos;

	int emptyHande;
	int maxHande;
	int timer;
	int finishTimer;
	bool drawEmptyFlag;
	bool drawMaxFlag;

	Vec2<float>lerpPlayerSize;

	bool shrinkFlag;
	bool extendFlag;

	bool extendMaxFlag;
	bool shrinkMaxFlag;

	float rate;

	float emptyRate;
	float fullRate;
};

