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


	void ParticleLevel(int NORMAL_PARTICLE_NUM, int SWING_PARTICLE_NUM);

private:
	std::array<StarParticle, 200>paticles;
	Vec2<float>pos, vel;
	int particleMax, particleNomalMax, nowParticleNum;
	int tracPatricle;
};

