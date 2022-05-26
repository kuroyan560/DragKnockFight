#include "StarParticleMgr.h"
#include"../Common/KuroFunc.h"
#include"CharacterManager.h"

StarParticleMgr::StarParticleMgr()
{
}

void StarParticleMgr::Init()
{
}

void StarParticleMgr::Update()
{
	if (CharacterManager::Instance()->Left()->GetNowSwing())
	{
		nowParticleNum = particleMax;
	}
	else
	{
		nowParticleNum = particleNomalMax;
	}


	float num = nowParticleNum - tracPatricle;
	tracPatricle += num * 0.1f;
	for (int i = 0; i < tracPatricle; ++i)
	{
		if (!paticles[i].isAlive())
		{
			float size = 200.0f;
			Vec2<float>emittPos = { pos.x - size / 2.0f,pos.y - size / 2.0f };
			Vec2<float>emittSize = { KuroFunc::GetRand(size),KuroFunc::GetRand(size) };


			float angel = 270 + KuroFunc::GetRand(180);
			Vec2<float>radian = { cosf(Angle::ConvertToRadian(angel)), sinf(Angle::ConvertToRadian(angel)) };
			Vec2<float>lVel = (Vec2<float>(0.0f, -1.2f));
			paticles[i].Init(emittPos + emittSize, lVel, KuroFunc::GetRand(120));
		}
	}

	for (int i = 0; i < paticles.size(); ++i)
	{
		paticles[i].Update();
	}
}

void StarParticleMgr::Draw()
{
	for (int i = 0; i < paticles.size(); ++i)
	{
		paticles[i].Draw();
	}
}

void StarParticleMgr::ParticleLevel(int NORMAL_PARTICLE_NUM, int SWING_PARTICLE_NUM)
{
	pos = CharacterManager::Instance()->Right()->pos;
	vel = CharacterManager::Instance()->Right()->vel;

	particleMax = SWING_PARTICLE_NUM;
	particleNomalMax = SWING_PARTICLE_NUM;
}
