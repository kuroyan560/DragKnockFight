#pragma once
#include"../KuroEngine.h"
#include"StarParticle.h"
#include<array>

class StarParticleMgr
{
public:
	StarParticleMgr();

	void Init();
	void Update();
	void Draw();

	Vec2<float>pos,vel;
	int particleMax;
	int tracPatricle;
private:
	std::array<StarParticle, 100>paticles;

};

