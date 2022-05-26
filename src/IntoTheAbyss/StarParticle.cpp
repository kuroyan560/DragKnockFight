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
}

void StarParticle::Update()
{
	if (initFlag)
	{
		pos += vel;

		float rate = static_cast<float>(timer) / static_cast<float>(finishTimer);
		size = { 1.0f - rate,1.0f - rate };


		if (finishTimer <= timer)
		{
			finishFlag = true;
			initFlag = false;
		}
		++timer;
	}
}

void StarParticle::Draw()
{
	if (initFlag)
	{
		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), size * ScrollMgr::Instance()->zoom, 0.0f, TexHandleMgr::GetTexBuffer(particleGraphHandle));
	}
}

bool StarParticle::isAlive()
{
	return initFlag;
}
