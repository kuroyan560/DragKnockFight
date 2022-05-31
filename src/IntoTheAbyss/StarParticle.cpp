#include "StarParticle.h"
#include"../Engine/DrawFunc.h"
#include"../IntoTheAbyss/TexHandleMgr.h"
#include"ScrollMgr.h"

bool StarParticle::initLoadGraphFlag = false;
int StarParticle::particleGraphHandle = -1;

StarParticle::StarParticle()
{
	if (!initLoadGraphFlag)
	{
		particleGraphHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/line_center.png");
		initLoadGraphFlag = true;
	}
	timer = -1;
	finishTimer = -1;
	initFlag = false;
}

void StarParticle::Init(const Vec2<float> &POS, const Vec2<float> &VEL, int FINISH_TIEMR)
{
	timer = 0;
	finishTimer = FINISH_TIEMR;
	pos = POS;
	vel = VEL;
	size = { 1.0f,1.0f };
	initFlag = true;
	finishFlag = false;
	alphaRad = 0.0f;
}

void StarParticle::Update()
{
	if (initFlag)
	{
		Vec2<float>lVel = { sinf(3.14f / 60.0f * timer) * 1.3f, vel.y };
		pos += lVel;

		float rate = static_cast<float>(timer) / static_cast<float>(finishTimer);
		size = { 1.0f - rate,1.0f - rate };

		if (finishTimer <= timer)
		{
			finishFlag = true;
			initFlag = false;
		}
		alphaRad += Angle::ConvertToRadian(0.1f);
		++timer;
	}
}

#include "GameSceneCamerMove.h"

void StarParticle::Draw()
{
	static const Vec2<float>BOX_SIZE = { 15,15 };
	if (initFlag)
	{
		//DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), size * ScrollMgr::Instance()->zoom, 0.0f, TexHandleMgr::GetTexBuffer(particleGraphHandle));
		const auto drawPos = ScrollMgr::Instance()->Affect(pos);
		const auto drawHalfSize = BOX_SIZE * size * ScrollMgr::Instance()->zoom * 0.5f * Vec2<float>(KuroFunc::GetRand(1.0f, 1.1f), KuroFunc::GetRand(1.0f, 1.1f));
		int alpha = 255 * cos(alphaRad);
		DrawFunc::DrawBox2D(drawPos - drawHalfSize + GameSceneCameraMove::Instance()->move, drawPos + drawHalfSize + GameSceneCameraMove::Instance()->move, Color(239, 1, 144, alpha), true, AlphaBlendMode_Trans);
	}
}

bool StarParticle::isAlive()
{
	return initFlag;
}
