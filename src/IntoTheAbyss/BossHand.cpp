#include "BossHand.h"
#include"ScrollMgr.h"
#include"../Engine/DrawFunc.h"
#include"TexHandleMgr.h"

BossHand::BossHand(int HANDLE)
{
	handGraph = HANDLE;
}

void BossHand::Init()
{
}

void BossHand::Update(const Vec2<float> &POS, float RADIUS, float RADIAN)
{
	centralPos = POS;

	//Žè‚ÌˆÊ’u
	//pos.x = centralPos.x + cosf(RADIAN) * RADIUS;
	//pos.y = centralPos.y + sinf(RADIAN) * RADIUS;

	pos.x = 50.0f;
	pos.y = 50.0f;
}

void BossHand::Draw()
{
	DrawFunc::DrawRotaGraph2D(pos, Vec2<float>(3.0f,3.0f), 0.0f, TexHandleMgr::GetTexBuffer(handGraph));
}
