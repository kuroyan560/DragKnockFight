#include "WinCounter.h"
#include"ScrollMgr.h"
#include"TexHandleMgr.h"

WinCounter::WinCounter()
{
	winCountGraph_Left = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/win_player.png");
	winCountGraph_Right = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/win_enemy.png");
	knockOutGraph_Left = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/knockOut_player.png");
	knockOutGraph_Right = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/knockOut_enemy.png");
}

void WinCounter::Update()
{
	if (!animation)return;
}

#include"DrawFunc.h"
#include"WinApp.h"
void WinCounter::Draw()
{
	//勝利数カウント(左)
	const Vec2<float>WIN_COUNT_POS = { 205,71 };
	const float WIN_COUNT_OFFSET_X = 35;
	for (int i = 0; i < 3; ++i)
	{
		const float offsetX = WIN_COUNT_OFFSET_X * i;
		DrawFunc::DrawGraph({ WIN_COUNT_POS.x + offsetX,WIN_COUNT_POS.y }, TexHandleMgr::GetTexBuffer(winCountGraph_Left), AlphaBlendMode_Trans);
	}

	//勝利数カウント(右)
	static const int WIN_COUNT_WIDTH = TexHandleMgr::GetTexBuffer(winCountGraph_Right)->GetDesc().Width;
	const float WIN_COUNT_RIGHT_X = WinApp::Instance()->GetExpandWinSize().x - WIN_COUNT_POS.x - WIN_COUNT_WIDTH * 4.5f;
	for (int i = 0; i < 3; ++i)
	{
		const float offsetX = WIN_COUNT_OFFSET_X * i;
		DrawFunc::DrawGraph({ WIN_COUNT_RIGHT_X + offsetX,WIN_COUNT_POS.y }, TexHandleMgr::GetTexBuffer(winCountGraph_Right), AlphaBlendMode_Trans);
	}
}

void WinCounter::RoundFinish(const Vec2<float>& FinishPos, const bool& WinnerIsLeft)
{
	countRound++;
	if (WinnerIsLeft)
	{
		countLeftWin++;
		knockOutDisappearPos = { 0,0 };
		knockOutGraph = knockOutGraph_Left;
	}
	else
	{
		countRightWin++;
		knockOutDisappearPos = { 1280,0 };
		knockOutGraph = knockOutGraph_Right;
	}

	const auto finishPosOnDraw = ScrollMgr::Instance()->Affect(FinishPos);
	knockOutAppearPos = finishPosOnDraw;

	knockOutTimer = 0;
	animation = true;
}
