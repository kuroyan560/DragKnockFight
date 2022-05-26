#include "StarParticleMgr.h"
#include"../Common/KuroFunc.h"

StarParticleMgr::StarParticleMgr()
{
}

void StarParticleMgr::Init()
{
}

void StarParticleMgr::Update()
{
	pos = { 500.0f,500.0f };
	for (int i = 0; i < paticles.size(); ++i)
	{
		if (!paticles[i].isAlive())
		{
			Vec2<float>angle(KuroFunc::GetRand(360.0f), KuroFunc::GetRand(360.0f));
			Vec2<float>radian(cosf(Angle::ConvertToRadian(angle.x)), sinf(Angle::ConvertToRadian(angle.y)));

			paticles[i].Init(pos, radian * 1.0f, 120);
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
