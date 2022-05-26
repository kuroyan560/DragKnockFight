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
	float size = 200.0f;

	float num = particleMax - tracPatricle;
	tracPatricle += num * 0.1f;


	for (int i = 0; i < tracPatricle; ++i)
	{
		if (!paticles[i].isAlive())
		{


			Vec2<float>emittPos = { pos.x - size / 2.0f,pos.y - size / 2.0f };
			Vec2<float>emittSize = { KuroFunc::GetRand(size),KuroFunc::GetRand(size) };

			//Vec2<float>angle(KuroFunc::GetRand(360.0f), KuroFunc::GetRand(360.0f));
			//Vec2<float>radian(cosf(Angle::ConvertToRadian(angle.x)), sinf(Angle::ConvertToRadian(angle.y)));
			//paticles[i].Init(emittPos+emittSize, radian * 0.0f, KuroFunc::GetRand(120));


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
