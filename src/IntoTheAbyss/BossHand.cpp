#include "BossHand.h"
#include"ScrollMgr.h"
#include"../Engine/DrawFunc.h"
#include"TexHandleMgr.h"

BossHand::BossHand()
{
	handGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/");
}

void BossHand::Init()
{
}

void BossHand::Update(const Vec2<float> &POS, float RADIUS, float RADIAN)
{
	centralPos = POS;

	//Žè‚ÌˆÊ’u
	pos.x = centralPos.x + cosf(RADIAN) * RADIUS;
	pos.y = centralPos.y + sinf(RADIAN) * RADIUS;
}

void BossHand::Draw()
{
	DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), Vec2<float>(1.0f,1.0f), inputAngle, TexHandleMgr::GetTexBuffer(handGraph));
}
