#pragma once
#include"../KuroEngine.h"

class StarParticle
{
public:
	StarParticle();

	void Init(const Vec2<float> &POS, const Vec2<float> &VEL, int FINISH_TIEMR);
	void Update();
	void Draw();

	bool isAlive();
private:
	static bool initLoadGraphFlag;
	static int particleGraphHandle;

	Vec2<float>pos, size, vel;

	int timer;
	int finishTimer;
	bool finishFlag;
	bool initFlag;
};

