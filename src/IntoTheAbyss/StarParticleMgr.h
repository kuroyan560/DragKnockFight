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


private:
	Vec2<float>pos;

	std::array<StarParticle, 100>paticles;

};

